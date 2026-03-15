#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcId(const char* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_stricmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

#include <string>
#include <codecvt>

std::wstring ExePath() {
	WCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

int main() {
	//const char* dllPath = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\DayZServer\\x64\\Debug\\TCI-DLL.dll";
	const char* procName = "DayZServer_x64.exe";
	DWORD procId = 0;

	std::wstring dllPathU = ExePath();
	dllPathU += L"\\TCI-DLL.dll";

	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::string converted_str = converter.to_bytes(dllPathU);

	//wprintf(dllPathU.c_str());

	//std::cout << sizeof(dllPathU);

	//std::cout << dllPath << std::endl;
	//std::cout << converted_str.c_str() << std::endl;

	//system("pause");return 0;

	while (!procId)
	{
		procId = GetProcId(procName);
		Sleep(30);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (loc)
		{
			WriteProcessMemory(hProc, loc, converted_str.c_str(), strlen(converted_str.c_str()) + 1, 0);
		}

		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

		if (hThread)
		{
			CloseHandle(hThread);
		}
	}

	if (hProc)
	{
		CloseHandle(hProc);
	}

	return 0;
}