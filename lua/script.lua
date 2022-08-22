
--policeMen = {}

ConsoleMessage("YESS")
BroadcastMessage("Lua Script loaded!")
BroadcastMessage("Lua Script loaded!")
BroadcastMessage("Lua Script loaded!")
ConsoleMessage("YESS555")


policeLocker = {12050.608398438, 6.77032995224, 3612.6008300781}

function distance(x1,y1,z1, x2,y2,z2)
	return math.sqrt((x1-x2)^2 + (y1-y2)^2 + (z1-z2)^2)
end

function distanceToLocker(x,y,z)
	return distance(x,y,z, policeLocker[1], policeLocker[2], policeLocker[3])
end

function locker(playerID)
	--BroadcastMessage("LOCKER COMMAND")
	
	x, y, z = GetPlayerPosition(playerID)
	local dist = distanceToLocker(x,y,z)
	ConsoleMessage("distanceToLocker " .. dist)
	if dist < 1 then
		BroadcastMessage("You are within the area of the locker")
	else
		BroadcastMessage("You are not near the locker")
	end
end
RegisterCommandHandler("/locker", locker)

--[[
function getPlayers(playerID)
	players = {12321, 52323, 5123123, 552111}

end
RegisterCommandHandler("/getPlayers", getPlayers)
]]

function com1(playerID)
	BroadcastMessage("com1 works! playerID " .. playerID)
	ConsoleMessage("com1 ran by " .. playerID)
end
RegisterCommandHandler("/com", com1)

function com2(playerID)
	
	x, y, z = GetPlayerPosition(playerID)

	BroadcastMessage(x .. " " .. y .. " " .. z)

	ConsoleMessage("com2 " .. x .. " " .. y .. " " .. z)
end
RegisterCommandHandler("/com2", com2)

function com3(playerID)
	BroadcastMessage("com3 works!")
	ConsoleMessage("com3 ran")
end
RegisterCommandHandler("/com3", com3)

activeCop = 0

function onDuty(playerID)
	BroadcastMessage("You are now hired as a cop!")
	activeCop = playerID
	ConsoleMessage("cop hired")
end
RegisterCommandHandler("/onDuty", onDuty)

function offDuty(playerID)
	BroadcastMessage("You are off duty!")
	activeCop = 0
	ConsoleMessage("cop released")
end
RegisterCommandHandler("/offDuty", offDuty)

function job(playerID)
	if playerID == activeCop then
		BroadcastMessage("You ARE the cop!")
	else
		BroadcastMessage("You are not even hired!")
	end
end
RegisterCommandHandler("/job", job)

ConsoleMessage("Loaded")

--[[
function doSomeStuff()
	BroadcastMessage("Lua Script loaded!")
	BroadcastMessage("Reloading works :P")
end

ConsoleMessage("frau frau frau frau frau")

ConsoleMessage("YESS")
BroadcastMessage("Lua Script loaded!")
BroadcastMessage("Lua Script loaded!")
BroadcastMessage("Lua Script loaded!")
ConsoleMessage("YESS555")
BroadcastMessage("it does really work.")
ConsoleMessage("3020YESS555")
BroadcastMessage("the time is " .. os.date('%X', os.time()))
ConsoleMessage("[error] TU A GEAU")
]]


--[[

function some(command, player)

	BroadcastMessage("Someone ran /some")
	SendPlayerMessage(player, "Ok")

end
RegisterCommandHandler("/some", some)

local KamyPoliceOutside = {12044.90, 3587.46}
onDutyPlayers = {}

function isPlayerOnDuty(player)
	-- Must implement...
	return false
end

function removeFromDuty(player)
	-- Must implement
end

function onDuty(command, player)
	if not isPlayerOnDuty(player) then
		table.insert(onDutyPlayers, player)
	end
end
RegisterCommandHandler("/onDuty", onDuty)

function offDuty(command, player)
	if isPlayerOnDuty(player) then
		removeFromDuty(player)
	end
end
RegisterCommandHandler("/offDuty", offDuty)

function isInPoliceHQRange(player)
	-- Must implement
	return true
end

function getLoadout(command, player)
	if isPlayerOnDuty(player) then
		if isInPoliceHQRange(player) then
			SpawnPlayerItem(player, "SKS", 1, true)
			SpawnPlayerItem(player, "AmmoBox_762x39_20Rnd", 2, true)
			SpawnPlayerItem(player, "PUScopeOptic", 1, true)
			SendPlayerMessage(player, "Loadout spawned!")
		else
			SendPlayerMessage(player, "You are not in range of a police HQ")
		end
	end
end
RegisterCommandHandler("/loadout", getLoadout)







lua script:
	something
	something2
	something3


/lua  -> 6[1012]         if ShouldHotReload => {Preserve}
				lua script...
			<- 6[220] something
bla bla bla
       ->6[220]               resume
			<- 6[220] something2
bla bla bla
	->6[220]             resume
			<- 6[220] something3
bla bla bla
	->6[220]
			    resume
			<- 6[0] .... (exit)
			INTERCEPT - if Preserved...
			popPreserve + set not preserved
			goIntoLua()
			<-6[1012] actual call..
whatever
	->6[1012]            resume
			<- 6[0] ... (exit)
			Not intercepting, nothing preserved
6[0]==0 => break
.

Done







]]