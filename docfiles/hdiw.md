# How does it work?

In case you are interested in a *story of how it was developed*, [click here](story.md).

Tony's Convenience Injector works, as the name implies, by injecting a `.dll` in the vanilla server.

The `.dll` is written in C++ and the project has an interop layer written in EnfusionScript.

The `.dll` can be injected with any capable dll injector or with the rather simple included `AutoInjector.exe`.

When the `.dll` file is injected into the server, it spawns 2 threads.
+ One thread sets up the bridge and detours server code execution (a short-lived thread)
+ One thread launches up the GUI (a 'permanent' thread)

The GUI thread is rather simple to understand, so we'll focus on the bridge & detour.

## Data bridge

The first priority is to data-bridge the connection between TCI and DayZ server. We do this by finding the location in memory of an EnfusionScript int array called `DLL_WATERMARK`. This contains a particular list of bytes that should be unique in memory.

```
Watermark bytes :
01 3B AB E1 B3 BC AF F2 E3 1B 26 98 73 72 BC AD

Written as :

DLL_WATERMARK[0] = -508871935;
DLL_WATERMARK[1] = -223363917;
DLL_WATERMARK[2] = -1742332957;
DLL_WATERMARK[3] = -1380158861;

in EnfusionScript.
```

The EnfusionScript part of TCI creates a file in the `$profile:` directory on init.

The file contains the **lowest 32 bits** of the pointer to the `DLL_WATERMARK` array. (Because ES truncates values to 32 bits by default).

The aforementioned C++ thread waits for that file to exist on disk, then reads it, and deletes it. Then, it proceeds to scan the entire memory range for the **higher 32 bits** and checking for the watermark.

```
Trying to find the full address HHHH HHHH LLLL LLLL, it scans through:

0000 0001 LLLL LLLL
0000 0002 LLLL LLLL
0000 0003 LLLL LLLL
...
FFFF FFFF LLLL LLLL
```

Once the watermark pattern is found somewhere, the bridge is set. TCI can now write data to that location in memory and it coincides with EnfusionScript memory.

## Code detour

After the data bridge is set, TCI proceeds to create a detour in the server code that redirects code execution to the C++ `OnGetControl` function. The detour is created at a specific point in the server's code. See [the story](story.md) later for how that was discovered.

In this process, a `volatile` dummy function called `ourFunct` is used for manually writing some `jmp` assembly that preserves context.

⚠️ Do not modify or delete the `ourFunct` function ⚠️

It is used as a *code cave* and must be left in the code, even if apparently nothing calls it. This is why it is marked as volatile. 

In the data bridge, there is a reserved int called `DLL_IS_MAGIC_CALL` that is set to the magic value of 54 if the intercepted EnfusionScript function call is a 'magic call'.

```
EnfusionScript:
    void MagicCall_Internal() {
        DLL_IS_MAGIC_CALL = 0;
    }

    void MagicCall() {
        DLL_IS_MAGIC_CALL = 54;
        MagicCall_Internal();
    }
```

In EnfusionScript, calling `MagicCall()` basically calls `OnGetControl()` in C++.

This works because the detour intercepts every ES function call, but when `MagicCall_Internal()` gets called, the `DLL_IS_MAGIC_CALL` flag is set to 54.

```
C++ :
    volatile void OnGetControl() {
        if (DLL_BRIDGE == NULL) return;
        if (*DLL_IS_MAGIC_CALL != 54) return;

        // .. actual code ..
    }
```

💡 Once the code detour is done, the bridge & detour thread terminates. The only still living threads are :
+ the GUI thread for the TCI console (ran by `TCI.dll`)
+ the EnfusionScript thread that executes the init.c code (ran by `DayZServer_x64.exe`)

## Lua thread

TCI spawns a Lua thread for interpreting Lua and context-preservation purposes. When Lua has to call a function in ES, it yields (it interrupts its own thread and waits for it to be resumed). When the Lua script is reloaded, the thread is terminated and a fresh one is created.

The Lua thread waits for the main thread with a [condition variable](https://en.cppreference.com/w/cpp/thread/condition_variable), and the main thread waits for Lua with a busy-wait *while loop*. (It doesn't work with a condition variable for some reason).

## What happens on a chat command event

Let's say a player types `/gimmeM4` in the chat.

### EnfusionScript part

The chat message event is intercepted (in EnfusionScript init.c) through :
```
bool Command(PlayerBase player, string command)
```
which has a `switch` that checks against some EnfusionScript-level cases. If none of the commands match, the default case of the switch calls `RedirectCommandToLua`. (This way you can still add ES-level commands that are compatible with TCI without relying on its Lua API).

This later calls `InterpreterCycle`. This is the function that, in a while-loop, gives control to TCI by calling `MagicCall` and awaits control back. It does this as long as there is still something to be processed, hence the while-loop.

### C++ part

Each time `MagicCall` in init.c is called, the C++ `OnGetControl` function is called. The latter calls the necessary Lua functions on the Lua thread and busy-waits for responses.

For control yields to or from Lua there are two functions (declared in `dll-lua.h`) :
+ `void goInLua();` - called in the main thread, it busy-waits the Lua thread.
+ `void goOutOfLua();` - called in the Lua thread, it condition-variable waits the main thread.

Both of these hold the thread they're run in busy until the other thread responds. (So really, even if the main thread and Lua thread are two distinct actual *multi-thread* threads, the code is run sequentially. The threads' sole purpose is to preserve the context between yields).

### Function calls between TCI and the rest

If you check the code, every function call or function return between ES and TCI is actually a yield, with the parameters being passed via the data bridge.

Take a look at this C++ code:

```
void BroadcastMessage(char* str) {
    strcpy(DLL_STRING, str);
    DLL_STRLEN_IN[0] = strlen(str);
    *DLL_COMMAND = 1002;
    goOutOfLua();
}
```
It puts the string payload (the message to be broadcasted in-game) in `DLL_STRING`, and the length of it in `DLL_STRLEN_IN` which are part of the data bridge. It also puts a *magic value* command code of 1002.

After all the parameters are ready, it yields control to EnfusionScript by calling `goOutOfLua()`.

EnfusionScript is surely inside the `InterpreterCycle` if any Lua code has just been ran, so it is inside the while-loop that checks for the current `DLL_COMMAND` code. It sees 1002, so it broadcasts the message with the given parameters from the data bridge.

```
// EnfusionScript
void InterpreterCycle() {
    while (DLL_COMMAND != 0) {
        MagicCall(); // this yields control to TCI and waits for response

        /* some code in between */

        if (DLL_COMMAND == 1002) { // BroadcastMessage command code

            // this makes a deep-copy of the string passed over the bridge
            string luaResponse = DLL_INBOUND_STRING.Substring(0, DLL_STRLEN_IN[0]);

            // this runs ES code that actually broadcasts the message to players
            SendGlobalMessage("Broadcast : " + luaResponse);
        }

        /* some more code afterwards */
}
```