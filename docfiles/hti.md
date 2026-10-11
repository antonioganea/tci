# Installation instructions

## Requirements
+ DayZ Server installed through Steam
+ A tool to unarchive files.

## Instructions

1) First, go to [releases](https://github.com/antonioganea/tci/releases) and download the latest release (TCI.zip).

2) Navigate to the folder where the server is located. A default install location looks something like this : `C:\Program Files (x86)\Steam\steamapps\common\DayZServer`. However, your install location might vary.

3) The file `\mpmissions\dayzOffline.chernarusplus\init.c` will be overwritten in the next step, so if you've made any manual modifications to it, you should make a backup ( renaming it to `init-old.c` is fine ). If you're using a fresh install or don't care about that file, no action is required in this step.

4) Extract the files from the downloaded *TCI.zip* archive and place them inside the root directory of DayZServer. When asked to replace files, press yes.

5) Open `start-injected.bat` with a text editor like notepad and make sure this line matches with reality :
```
set serverLocation="C:\Program Files (x86)\Steam\steamapps\common\DayZServer"
```
(If you have a custom install location, you need to change this line accordingly. Otherwise, for the default install location under `C:`, no action is required during this step.)

6) IMPORTANT! Marking yourself as admin : Navigate to `C:\Users\{YOUR_USERNAME_HERE}\AppData\Local\DayZ` and locate the admins.txt file. You need to add your SteamID64 on a separate line. Save the file and reboot the server. (It's a one-time setup). This will allow you to run text-chat commands.

7) That's all.

If you have trouble installing it, don't shy away from contacting me on [Discord](https://discord.gg/3vs8S27kyV).

## Running the server

You have to double click on `start-injected.bat`. This will boot the server and the auto-injector that will load the DLL in-memory. An User-Access-Control prompt will pop up asking if you want to allow the auto-injector to run on your device with elevated privilleges. Hit yes. The privilleges are required for the injector to modify the memory of another process (DayZServer_x64.exe) and load the DLL.

For convenience, you can create a shortcut for `start-injected.bat` on your desktop.

## Why should I trust the injector with elevated privilleges?
You don't have to trust it. The source code is in the repository and it can be read in a couple of minutes because it is under 100 lines of code. You can compile it for yourself or use any other injector.

The injector is just there for convenience and correct timing.

Alternatively, you can manually use Cheat Engine to inject it (that also uses elevated privilleges). Open CE, attach it to the server process, open the Memory Viewer, go to Tools > Inject DLL, choose the file TCI-DLL.dll and it will ask if "you want to execute a function of the dll". Hit yes.