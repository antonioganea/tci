#include "bridge.h"

BYTE* DLL_BRIDGE = NULL;

/* BRIDGE LAYOUT */

int* DLL_DETOURED;
int* DLL_IS_MAGIC_CALL;
int* DLL_COMMAND;
int* DLL_TRIGGER_DEBUG_CALL;

int* DLL_INTN_IN;
int* DLL_INTN_OUT;

int* DLL_INTS_IN; // array
int* DLL_INTS_OUT; // array

int* DLL_FLOATN_IN;
int* DLL_FLOATN_OUT;

float* DLL_FLOATS_IN; // array
float* DLL_FLOATS_OUT; // array

int* DLL_STRN_IN;
int* DLL_STRN_OUT;
int* DLL_STRLEN_IN; // array
int* DLL_STRLEN_OUT; // array


char** DLL_STRING_LOC = NULL;
char* DLL_STRING = NULL;

char** DLL_IN_STR1 = NULL;
char** DLL_IN_STR2 = NULL;
char** DLL_IN_STR3 = NULL;
char** DLL_IN_STR4 = NULL;

/* END OF BRIDGE LAYOUT */

std::wstring bridgeFile;

#include <KnownFolders.h>
#include <shlobj.h>

void ComputeBridgeFilePath() {
    PWSTR localAppDataPath;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppDataPath);

    bridgeFile = localAppDataPath;
    bridgeFile += L"\\DayZ\\testfile.txt";
}

#include <fstream>

void SetupDLLBridge() {
    std::ifstream MyReadFile(bridgeFile);
    ////std::ifstream MyReadFile("C:\\Users\\Antonio\\Desktop\\VictimWithFile\\testfile.txt");

    unsigned int temp;

    MyReadFile >> temp;

    MyReadFile.close();

    TConv t;
    t.big = 0;
    t.t.low = temp;


    ////std::ofstream MyOutputFile("C:\\Users\\Antonio\\Desktop\\VictimWithFile\\testfile-response.txt");
//std::ofstream MyOutputFile("C:\\Users\\Antonio\\AppData\\Local\\DayZ\\testfile-response.txt");
// MyOutputFile << std::hex << temp << std::dec << std::endl;


// 01 3B AB E1 B3 BC AF F2 E3 1B 26 98 73 72 BC AD
    BYTE watermark[] = { 0x01, 0x3B, 0xAB, 0xE1, 0xB3, 0xBC, 0xAF, 0xF2, 0xE3, 0x1B, 0x26, 0x98, 0x73, 0x72, 0xBC, 0xAD };

    //MyOutputFile << "X" << std::flush;

    BYTE readBuffer[1000];
    SIZE_T bytesRead;

    HANDLE currentProcess = GetCurrentProcess();

    for (unsigned int i = 0; i <= 0xFFFFFFFF; i++) { // unsigned int ??? - might not work 

        bool success = ReadProcessMemory(currentProcess, (LPCVOID)t.big, &readBuffer, sizeof(watermark), &bytesRead);

        if (success) {
            //MyOutputFile << " Success : ";
            if (memcmp((void*)t.big, watermark, sizeof(watermark)) == 0) {
                DLL_BRIDGE = (BYTE*)t.big;
                //MyOutputFile << "Worked. Hooked." << std::hex << (long long) DLL_BRIDGE << std::dec << std::flush;
                break;
            }
        }

        t.t.high++;
    }

    //MyOutputFile << "-END OF FUNC-" << (long long)DLL_BRIDGE << std::flush;
    //                                     ^^^^^^ this was necessary

    //MyOutputFile.close();

    BYTE* ptr = DLL_BRIDGE + 4 * sizeof(int);

    DLL_DETOURED = (int*)ptr; ptr += sizeof(int);
    DLL_IS_MAGIC_CALL = (int*)ptr; ptr += sizeof(int);
    DLL_COMMAND = (int*)ptr; ptr += sizeof(int);
    DLL_TRIGGER_DEBUG_CALL = (int*)ptr; ptr += sizeof(int);

    DLL_INTN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_INTN_OUT = (int*)ptr; ptr += sizeof(int);

    //ptr += 4; // QWORD alignment

    ptr += 8; // ES Array header
    DLL_INTS_IN = (int*)ptr; ptr += 8 * sizeof(int);

    ptr += 8; // ES Array header
    DLL_INTS_OUT = (int*)ptr; ptr += 8 * sizeof(int);

    DLL_FLOATN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_FLOATN_OUT = (int*)ptr; ptr += sizeof(int);

    ptr += 8; // ES Array header
    DLL_FLOATS_IN = (float*)ptr; ptr += 8 * sizeof(float);

    ptr += 8; // ES Array header
    DLL_FLOATS_OUT = (float*)ptr; ptr += 8 * sizeof(float);

    DLL_STRN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_STRN_OUT = (int*)ptr; ptr += sizeof(int);

    ptr += 8; // ES Array header
    DLL_STRLEN_IN = (int*)ptr; ptr += 4 * sizeof(int);

    ptr += 8; // ES Array header
    DLL_STRLEN_OUT = (int*)ptr; ptr += 4 * sizeof(int);

    //Setting up the DLL_STRING bridge
    DLL_STRING_LOC = (char**)ptr; ptr += 8;
    DLL_STRING = *DLL_STRING_LOC;

    DLL_IN_STR1 = (char**)ptr; ptr += 8;
    DLL_IN_STR2 = (char**)ptr; ptr += 8;
    DLL_IN_STR3 = (char**)ptr; ptr += 8;
    DLL_IN_STR4 = (char**)ptr; ptr += 8;

    // TODO : Sometimes, there has been a spacer (8 bytes) observed between STRLEN_OUT and DLL_STRING_LOC in memory...

    strcpy(DLL_STRING, "Ok this is an interesting string");
}