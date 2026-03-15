#pragma once

void CreateDetour();

//void Detour();



/*

40 55 57 48 81 EC E8 00 00 00 48 8D 6C 24 20

FF 25 00 00 00 00 10 25 B1 B6 FC 7F 00 00 90

1  2  3  4  5  6  7  8  9  10 11 12 13 14 15

relative jmp
jmp qword ptr [injectionVictim.funcB]
FF 25 4A 00 00 00

4A - 50


// func a - base diff
7fffa27a24b0
7ff720a20000

*/



/*

Register preservation instructions:
( can be optimized for sure )

50 53 51 52 56 57 55 54 41 50 41 51 41 52 41 53 41 54 41 55 41 56 41 57 9C //pushing
90 // nop
9D 41 5F 41 5E 41 5D 41 5C 41 5B 41 5A 41 59 41 58 5C 5D 5F 5E 5A 59 5B 58 //popping

^ Might be incorrect - pushfq acts on "eflags" ... maybe it doesn't include the normal flags - needs googling

*/