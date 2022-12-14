#include "bridge.h"

#include <KnownFolders.h>
#include <shlobj.h>

// Returns something like : "C:\\Users\\Antonio\\AppData\\Local\\DayZ\\testfile.txt";
std::wstring GetBridgeFilePath() {
    std::wstring bridgeFile;

    PWSTR localAppDataPath;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppDataPath);

    bridgeFile = localAppDataPath;
    bridgeFile += L"\\DayZ\\testfile.txt";

    return bridgeFile;
}

#include <fstream>

unsigned int readBridgeFileValue(std::wstring filePath) {
    std::ifstream MyReadFile(filePath);
    unsigned int temp;
    MyReadFile >> temp;
    MyReadFile.close();
    return temp;
}

// TODO : return bool, and add error handling
void ScanForWatermark(unsigned int lowIntVal) {
    TConv t;
    t.big = 0;
    t.t.low = lowIntVal;

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
}

// TODO : return bool, and add error handling in dllmain.cpp
void SetupDataBridge() {
    std::wstring bridgeFile = GetBridgeFilePath();

    while (!fileExistsTest(bridgeFile)) { // maybe break after some time if this doesn't work.......
        Sleep(100);
    } // TODO : print to console when file was found...
    
    unsigned int lowInt = readBridgeFileValue(bridgeFile);

    // TODO : add error handling here
    ScanForWatermark(lowInt); // this sets DLL_BRIDGE correctly

    if (DLL_BRIDGE == NULL) {
        // TODO : error handling
    }

    bindBridgeLayout();

    if (_wremove(bridgeFile.c_str()) != 0) {
        // TODO: Add some console logs
#ifdef DESKTOP_DEBUG_FILE
        MyOutputFile << "bridge file successfully deleted\n" << std::flush;
#endif
    }
    else {
        // TODO: Add some console logs
#ifdef DESKTOP_DEBUG_FILE
        MyOutputFile << "bridge file couldn't be deleted\n" << std::flush;
#endif
    }
}