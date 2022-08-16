
--policeMen = {}

function onDuty()

	--x, y, z = GetPlayerPos(player)

	--if ... KamyPoliceOutside

	--table.insert(policeMen, player)

	BroadcastMessage("Random number " .. math.random(1, 10))
	BroadcastMessage("You should take care of the guns")

	BroadcastMessage("The result is " .. (10^3))

	for i = 1, 3 do

		BroadcastMessage("crick")

	end

	-- GiveItem("sks", player)
end

onDuty()


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