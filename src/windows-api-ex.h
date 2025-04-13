﻿
//==============================================================================
//
//   windows-api-ex.h 
//
//==============================================================================
//  Copyright (C) Guilaume Plante 2020 <cybercastor@icloud.com>
//==============================================================================


#ifndef WINAPIEX_H
#define WINAPIEX_H


#include <fstream>
#include <Windows.h>
#include <winternl.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <lm.h>
#include <Psapi.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>


#ifdef UNICODE
#define LOADLIBRARY "LoadLibraryW"
#else
#define LOADLIBRARY "LoadLibraryA"
#endif
#define TIMEOUT_10SEC 10000
#define QUITE_LARGE_NB_PROCESSES 256
#define SOME_SYSTEM_PROCESS_IN_CURRENT_SESSION _T("winlogon.exe")
#define INJECTION_RIGHTS (PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE)
#define IDENTIFICATION_RIGHTS (PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_READ)

namespace C
{
	//TODO: Review checking of invalid handles (NULL vs INVALID_HANDLE_VALUE, INVALID_SOMETHING_HANDLE, etc.)
	//FEATURE: Injection bootstrapper for .NET DLL's

	template<typename T>
	struct Array
	{
	private:
		int _Capacity = 0;
		int _Count = 0;
		T* _Values = NULL;

		void __ctor(int capacity)
		{
			_Capacity = capacity;
			_Values = new T[_Capacity];
		}
	public:
		Array()
		{
			__ctor(0);
		}
		Array(int capacity)
		{
			__ctor(capacity);
		}

		int Count()
		{
			return _Count;
		}
		void Add(const T &value)
		{
			if (_Count == _Capacity)
			{
				_Capacity = (_Capacity & ~0xff) + 0x100;

				T *newValues = new T[_Capacity];
				memcpy(newValues, _Values, sizeof(T) * _Count);

				delete[] _Values;
				_Values = newValues;
			}

			_Values[_Count++] = value;
		}

		const T& operator [](int index)
		{
			if (index < 0 || index >= _Count) throw;
			return _Values[index];
		}
	};

	enum class SpecialFolder
	{
		Desktop = CSIDL_DESKTOP,
		Internet = CSIDL_INTERNET,
		Programs = CSIDL_PROGRAMS,
		Controls = CSIDL_CONTROLS,
		Printers = CSIDL_PRINTERS,
		MyDocuments = CSIDL_MYDOCUMENTS,
		Favorites = CSIDL_FAVORITES,
		Startup = CSIDL_STARTUP,
		Recent = CSIDL_RECENT,
		SendTo = CSIDL_SENDTO,
		BitBucket = CSIDL_BITBUCKET,
		StartMenu = CSIDL_STARTMENU,
		MyMusic = CSIDL_MYMUSIC,
		MyVideo = CSIDL_MYVIDEO,
		DesktopDirectory = CSIDL_DESKTOPDIRECTORY,
		Drives = CSIDL_DRIVES,
		Network = CSIDL_NETWORK,
		NetHood = CSIDL_NETHOOD,
		Fonts = CSIDL_FONTS,
		Templates = CSIDL_TEMPLATES,
		CommonStartMenu = CSIDL_COMMON_STARTMENU,
		CommonPrograms = CSIDL_COMMON_PROGRAMS,
		CommonStartup = CSIDL_COMMON_STARTUP,
		CommonDesktopDirectory = CSIDL_COMMON_DESKTOPDIRECTORY,
		AppData = CSIDL_APPDATA,
		PrintHood = CSIDL_PRINTHOOD,
		LocalAppData = CSIDL_LOCAL_APPDATA,
		AltStartup = CSIDL_ALTSTARTUP,
		CommonAltStartup = CSIDL_COMMON_ALTSTARTUP,
		CommonFavorites = CSIDL_COMMON_FAVORITES,
		InternetCache = CSIDL_INTERNET_CACHE,
		Cookies = CSIDL_COOKIES,
		History = CSIDL_HISTORY,
		CommonAppData = CSIDL_COMMON_APPDATA,
		Windows = CSIDL_WINDOWS,
		System = CSIDL_SYSTEM,
		ProgramFiles = CSIDL_PROGRAM_FILES,
		MyPictures = CSIDL_MYPICTURES,
		Profile = CSIDL_PROFILE,
		SystemX86 = CSIDL_SYSTEMX86,
		ProgramFilesX86 = CSIDL_PROGRAM_FILESX86,
		ProgramFilesCommon = CSIDL_PROGRAM_FILES_COMMON,
		ProgramFilesCommonX86 = CSIDL_PROGRAM_FILES_COMMONX86,
		CommonTemplates = CSIDL_COMMON_TEMPLATES,
		CommonDocuments = CSIDL_COMMON_DOCUMENTS,
		CommonAdminTools = CSIDL_COMMON_ADMINTOOLS,
		AdminTools = CSIDL_ADMINTOOLS,
		Connections = CSIDL_CONNECTIONS,
		CommonMusic = CSIDL_COMMON_MUSIC,
		CommonPictures = CSIDL_COMMON_PICTURES,
		CommonVideo = CSIDL_COMMON_VIDEO,
		Resources = CSIDL_RESOURCES,
		ResourcesLocalized = CSIDL_RESOURCES_LOCALIZED,
		CommonOemLinks = CSIDL_COMMON_OEM_LINKS,
		CDBurnArea = CSIDL_CDBURN_AREA,
		ComputersNearMe = CSIDL_COMPUTERSNEARME
	};

	namespace Convert
	{

		// The following 2 functions (allocate_argn/release_argn) with utils wstrlen/dup are used to convert WSTR (unicode) Main Argv parameter to char. 
		// This is for example used for cmdline parsing
		int wstrlen(_TCHAR* wstr);
		char* wstrdup(_TCHAR* wSrc);
		char** allocate_argn(int argc, _TCHAR* argv[]);
		void release_argn(int argc, char** nargv);

		namespace base64 {
			const std::string encode64(const std::string& input);
			const std::string decode64(const std::string& input);
		}
		LPWSTR StringToString(LPCSTR str);
		LPSTR StringToString(LPCWSTR str);

		LPWSTR Int32ToString(__int32 value, int base = 10);
		LPWSTR UInt32ToString(unsigned __int32 value, int base = 10);
		LPWSTR Int64ToString(__int64 value, int base = 10);
		LPWSTR UInt64ToString(unsigned __int64 value, int base = 10);
		LPWSTR FloatToString(float value);
		LPWSTR DoubleToString(double value);

		__int32 StringToInt32(LPCWSTR str, int base = 10);
		unsigned __int32 StringToUInt32(LPCWSTR str, int base = 10);
		__int64 StringToInt64(LPCWSTR str, int base = 10);
		unsigned __int64 StringToUInt64(LPCWSTR str, int base = 10);
		float StringToFloat(LPCWSTR str);
		double StringToDouble(LPCWSTR str);

		LPWSTR UInt32ToHexString(unsigned __int32 value);
		LPWSTR UInt64ToHexString(unsigned __int64 value);
		LPWSTR BytesToHexView(LPBYTE data, DWORD length);
	}

	namespace Random
	{
		LPBYTE Bytes(DWORD length);
		LPWSTR HexadecimalString(DWORD length);
	}

	namespace Environment
	{
		LPWSTR GetCurrentUser();
		BOOL GetWindowsVersion(LPDWORD major, LPDWORD minor);
		Array<LPWSTR>* GetCommandLineArgs();
		LPWSTR GetTimeStamp(BOOL useFileFormat = FALSE);
	}

	namespace Path
	{
		LPWSTR Combine(LPCWSTR path1, LPCWSTR path2);
		LPWSTR GetDirectoryName(LPCWSTR path);
		LPWSTR GetFileName(LPCWSTR path);

		LPWSTR GetExecutablePath();
		LPWSTR GetStartupPath();
		LPWSTR GetModulePath(HMODULE module);
		LPWSTR GetTempFolderPath();
		LPWSTR GetFolderPath(SpecialFolder folder);
	}

	namespace Directory
	{
		BOOL Exists(LPCWSTR path);
		Array<LPWSTR>* GetDirectories(LPCWSTR path);
		Array<LPWSTR>* GetFiles(LPCWSTR path);
	}

	namespace File
	{
		BOOL Exists(LPCWSTR path);
		LPBYTE Read(LPCWSTR path, LPDWORD bytesRead);
		BOOL Write(LPCWSTR path, LPBYTE data, DWORD length);
	}

	namespace Registry
	{
		LPWSTR GetValueString(HKEY hive, LPCWSTR subkey, LPCWSTR name);
		BOOL GetValueDword(HKEY hive, LPCWSTR subkey, LPCWSTR name, PDWORD value);
		BOOL SetValueString(HKEY hive, LPCWSTR subkey, LPCWSTR name, LPCWSTR value, BOOL isExpandedString = FALSE);
		BOOL SetValueDword(HKEY hive, LPCWSTR subkey, LPCWSTR name, DWORD value);
		BOOL DeleteValue(HKEY hive, LPCWSTR subkey, LPCWSTR name);

		BOOL CreateKey(HKEY hive, LPCWSTR subkey, LPCWSTR name);
		BOOL DeleteKey(HKEY hive, LPCWSTR subkey, LPCWSTR name);
		Array<LPWSTR>* GetKeyNames(HKEY hive, LPCWSTR subkey);
		Array<LPWSTR>* GetValueNames(HKEY hive, LPCWSTR subkey);
	}

#define STATUS_SUCCESS    ((NTSTATUS)0x00000000L)
#define ThreadQuerySetWin32StartAddress 9
	namespace Process
	{
		typedef struct _CLIENT_ID {
			HANDLE UniqueProcess;
			HANDLE UniqueThread;
		} CLIENT_ID;

		typedef struct _THREAD_BASIC_INFORMATION {
			NTSTATUS    exitStatus;
			PVOID       pTebBaseAddress;
			CLIENT_ID   clientId;
			KAFFINITY	AffinityMask;
			int			Priority;
			int			BasePriority;
			int			v;

		} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

		typedef enum _SC_SERVICE_TAG_QUERY_TYPE {
			ServiceNameFromTagInformation = 1,
			ServiceNameReferencingModuleInformation,
			ServiceNameTagMappingInformation,
		} SC_SERVICE_TAG_QUERY_TYPE, * PSC_SERVICE_TAG_QUERY_TYPE;

		typedef struct _SC_SERVICE_TAG_QUERY {
			ULONG   processId;
			ULONG   serviceTag;
			ULONG   reserved;
			PVOID   pBuffer;
		} SC_SERVICE_TAG_QUERY, * PSC_SERVICE_TAG_QUERY;

		struct SModuleEntry
		{
			std::string imageName;
			std::string moduleName;
			DWORD baseAddress;
			DWORD size;
		};
		struct SThreadEntry
		{
			DWORD threadId;
			DWORD parentPid;
			DWORD baseAddress;
			DWORD size;
			DWORD flags;
		};
		typedef std::vector<SModuleEntry> TModules;
		typedef TModules::iterator ModuleListIter;
		typedef std::vector<SThreadEntry> TThreads;
		typedef TThreads::iterator ThreadsListIter;

		typedef struct _MODULEINFO {
			LPVOID lpBaseOfDll;
			DWORD SizeOfImage;
			LPVOID EntryPoint;
		} MODULEINFO, * LPMODULEINFO;
		//  Forward declarations:

		typedef LONG    NTSTATUS;
		typedef NTSTATUS(WINAPI* pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);

		//FEATURE: GetAllProcesses()
		//FEATURE: BOOL EnableDebugPrivilege();
		LPCWSTR GetIntegrityLevelName(DWORD integrityLevel);
		BOOL EnableDebugPrivilege();
		DWORD GetProcessIdByName(LPCWSTR name);
		BOOL CreateProcessWithIntegrity(LPCWSTR commandLine, DWORD integrityLevel, LPDWORD processId);
		BOOL TerminateProcess(DWORD processId, UINT uExitCode);
		LPWSTR GetProcessName(DWORD processId);
		LPWSTR GetProcessCommandLine(DWORD processId);
		DWORD GetProcessIntegrityLevel(HANDLE process);
		DWORD GetParentProcessId(DWORD processId);
		Array<HWND>* GetProcessWindows(DWORD processID);
		BOOL InjectDll(HANDLE process, LPCWSTR dllPath);
		BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
		// Privilage Elevation

		DWORD GetCurrentSessionId();
		BOOL CreateInteractiveProcessForUser(LPTSTR lpszUsername, LPTSTR lpszDomain, LPTSTR lpszPassword, LPTSTR lpCommandLine);
		BOOL CreateInteractiveProcess(TCHAR* pszCommandLine);
		BOOL ProcessIdToName(DWORD processId, TCHAR* processName, DWORD buffSize);
		BOOL GetLogonSID(HANDLE hToken, PSID *ppsid);
		BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid);
		BOOL AddAceToDesktop(HDESK hdesk, PSID psid);
		BOOL FillProcessesListWithAlloc(PDWORD*, DWORD, PDWORD);
		DWORD FillProcessesList(PDWORD*, DWORD);
		BOOL GetProcessbyNameOrId(LPTSTR searchstring, PHANDLE phProcess, DWORD rights);
		DWORD GetProcessSession(HANDLE hProcess);
		BOOL IsRunAsAdministrator();
		void ElevateNow(int argc, const TCHAR *argv, TCHAR envp);
		bool RunAsAdministrator(const std::string& command, const std::string& arguments);
		bool RunPowerShellCmdAsAdministrator(const std::string& Command);
		BOOL EnableRequiredPrivileges();
		bool FillModuleListPSAPI(TModules& mods, DWORD pid, HANDLE hProcess);
		bool FillModuleListTH32(C::Process::TModules& modules, DWORD pid);
		BOOL ListProcessThreads(DWORD dwOwnerPID, TThreads & t);
		void printError(TCHAR* msg);
		DWORD GetThreadStartAddress(HANDLE hThread);
	}

	namespace Thread
	{
		DWORD WINAPI GetThreadStartAddress(HANDLE hThread);
	}

	namespace Service
	{
		//FEATURE: GetAllServices()
		SC_HANDLE GetServiceByName(LPCWSTR name);
		DWORD GetServiceState(SC_HANDLE service);
		DWORD GetServiceProcessId(SC_HANDLE service);
		BOOL StartServiceWait(SC_HANDLE service, DWORD expectedState, DWORD delayMilliseconds, DWORD timeoutMilliseconds);
		BOOL ControlServiceWait(SC_HANDLE service, DWORD control, DWORD expectedState, DWORD delayMilliseconds, DWORD timeoutMilliseconds);
	}

	namespace FileOperation
	{
		BOOL Copy(LPCWSTR srcPath, LPCWSTR destPath);
		BOOL Move(LPCWSTR srcPath, LPCWSTR destPath);
		BOOL Delete(LPCWSTR path);
	}

	namespace Message
	{
		void Information(LPCWSTR message);
		void Information(LPCWSTR title, LPCWSTR message);
		void Warning(LPCWSTR message);
		void Warning(LPCWSTR title, LPCWSTR message);
		void Error(LPCWSTR message, BOOL exitProcess = FALSE);
		void Error(LPCWSTR title, LPCWSTR message, BOOL exitProcess = FALSE);
		BOOL Confirmation(LPCWSTR message, BOOL warning = FALSE);
		BOOL Confirmation(LPCWSTR title, LPCWSTR message, BOOL warning = FALSE);
	}
}

#endif