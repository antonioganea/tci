# TCI Lua API


```
ConsoleMessage("yourMessageHere")

// Prints a string to the TCI GUI console.
```

```
BroadcastMessage("yourMessageHere")

// Broadcasts a red chat message to all players on the server.
```


```
SendPlayerMessage(myPlayer, "yourMessageHere")

// Sends a red chat message to the specified player.
// Check below to see how to get the specified player variable.
```

```
car = SpawnCar("olga", x, y, z)

// Spawns a car of specified type at location x, y, z
// Valid car types are : "olga", "offroad', "olgablack", "sarka", "gunter"
// x, y, z are float values
// Returns car variable
```

```
car = GetPlayerCar(playerID)
// Get the car the specified player is in
// Check below to see how to get the specified player variable.
// Returns car variable
```

```
playerCount = GetPlayerCount()

// Returns an integer number that is the current server player count
```

```
player = GetPlayerBySteamID(steamID)

// Gets a player by its steamID64 decimal ID
// Returns player variable or 0 if not found
```

```
steamID = GetPlayerSteamID(myPlayer)

// Returns steamID64 decimal ID of specified player
```

```
health = GetPlayerHealth(myPlayer)

// Returns float value of the specified player's health. It is a float.
```

```
maxHealth = GetPlayerMaxHealth(myPlayer)

// Returns float value of the specified player's maximum health. It is a float.
```

```
SetPlayerHealth(myPlayer, health+10)

// Set specified player's health.
// Second parameter is the health value to be set. It is a float.
```

```
blood = GetPlayerBlood(myPlayer)

// Returns float value of the specified player's blood. It is a float.
```

```
maxBlood = GetPlayerMaxBlood(myPlayer)
```

```
SetPlayerBlood(myPlayer, blood+10)
```

```
shock = GetPlayerShock(myPlayer)
```

```
maxShock = GetPlayerMaxShock(myPlayer)
```

```
SetPlayerShock(myPlayer, shock+10)
```

```
KillPlayer(myPlayer)
```

```
x, y, z = GetPlayerPosition(myPlayer)
// Returns specified player's position as x, y, z float coordinates
```

```
SetPlayerPosition(myPlayer, x, y, z)
// Sets specified player's position to x, y, z float coordinates
// Useful for teleports
```

```
fuel = GetCarFuel(car)
// Returns the fuel value the specified car has inside. (float)
```

```
SetCarFuel(car, fuel)
// Sets the fuel float value of the specified car.
```

```
capacity = GetCarFuelCapacity(car)
// Returns car capacity of the specified car as a float value.
```

```
isdriving = IsPlayerDriving(playerID)
// Returns true if the specified player is driving, false otherwise
```

```
seatno = GetPlayerSeatNumber(playerID)
// Returns the seat number of the specified player within a car
// Returns 0 if the player is not in a car
```

```
SpawnPlayerItem(myPlayer, "SKS", 3, true)
// Last parameter determines if the item is spawned in inventory or on the floor
// true spawns in inventory, false spawns on floor
```

```
itemStack = GetPlayerItemStackInHand(myPlayer)
// NOT IMPLEMENTED YET
```

```
itemName = GetItemStackName(itemStack)
// NOT IMPLEMENTED YET
```

```
RegisterCommandHandler("/onduty", onDutyHandler)
// Registers a command handler (callback) for a specific function
// The callback function is passed the playerID that called the command
// The commands are case-sensitive. (/onDuty is different from /onduty)

Example :

function cutGas(playerID)
	car = GetPlayerCar(playerID)
	SetCarFuel(car, 0)
end
RegisterCommandHandler("/cutGas", cutGas)
```

```
UnregisterCommandHandler("/onduty", onDutyHandler)
// NOT IMPLEMENTED YET
```

```
ClearCommandHandlers("/onduty")
// NOT IMPLEMENTED YET
```

```
RegisterEventHandler("onPlayerKilled", onKilledHandler)
// event types : "onStartingEquipSetup", "onPlayerKilled", "onUpdate"
// NOT IMPLEMENTED YET
```

```
CallLater(1000, callback, repeated?=false, param1, param2, param3 ..)
// NOT IMPLEMENTED YET
```