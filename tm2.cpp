#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>
#include <locale.h>

#define UNICODE
#define _UNICODE


using namespace std;

void ErrorExit(LPTSTR lpszFunction, DWORD dw)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	printf("%s\n", (LPCTSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

typedef struct _REG_TZI_FORMAT
{
	LONG Bias;
	LONG StandardBias;
	LONG DaylightBias;
	SYSTEMTIME StandardDate;
	SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

int main()
{
	DYNAMIC_TIME_ZONE_INFORMATION tziNew;
	DWORD dwRet; 
	
	REG_TZI_FORMAT regTZI;

	DWORD dwBufLen = sizeof(regTZI);
	LONG lRet;

	// Enable the required privilege

	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_TIME_ZONE_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	// Retrieve the current time zone information
	
	HKEY hKey;

	//setlocale(LC_ALL, "chs");

	LPCWSTR sub = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\Eastern Standard Time";
	LPCWSTR dtm_key = L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\Eastern Standard Time\\Dynamic DST";
	lRet = RegOpenKeyExW(HKEY_LOCAL_MACHINE, sub, 0, KEY_QUERY_VALUE, &hKey);
	printf("%d\n", lRet);
	if (lRet != ERROR_SUCCESS) {
		ErrorExit(TEXT("RegOpenKeyExA"), 2L);
		return 0;
	}

	printf("begin read value\n");
	lRet = RegQueryValueExW(hKey, L"TZI", NULL, NULL, (LPBYTE)&regTZI, &dwBufLen);
	
	if (lRet != ERROR_SUCCESS) {
		printf("error!");
		ErrorExit(TEXT("RegQueryValueExA"), GetLastError());
		return 0;
	}
	
	DWORD dwBufSize = 32 * sizeof(WCHAR);
	
	if (RegQueryValueExW(hKey, L"Dlt", NULL, NULL, (LPBYTE)tziNew.DaylightName, &dwBufSize) == ERROR_SUCCESS)
		wprintf(L"Dlt:%s\n", tziNew.DaylightName);

	if (RegQueryValueExW(hKey, L"Std", NULL, NULL, (LPBYTE)tziNew.StandardName, &dwBufSize) == ERROR_SUCCESS)
		wprintf(L"Std:%s\n", tziNew.StandardName);

	RegCloseKey(hKey);
	

	// Adjust the time zone information

	ZeroMemory(&tziNew, sizeof(tziNew));
	tziNew.DynamicDaylightTimeDisabled = FALSE;
	tziNew.TimeZoneKeyName;
	lstrcpyW(tziNew.TimeZoneKeyName, dtm_key);
	tziNew.Bias = regTZI.Bias;
	tziNew.Bias = regTZI.Bias;
	tziNew.StandardBias = regTZI.StandardBias;
	tziNew.DaylightBias = regTZI.DaylightBias;
	tziNew.StandardDate.wYear = regTZI.StandardDate.wYear;
	tziNew.StandardDate.wMonth = regTZI.StandardDate.wMonth;
	tziNew.StandardDate.wDay = regTZI.StandardDate.wDay;
	tziNew.StandardDate.wDayOfWeek = regTZI.StandardDate.wDayOfWeek;
	tziNew.StandardDate.wHour = regTZI.StandardDate.wHour;
	tziNew.StandardDate.wMinute = regTZI.StandardDate.wMinute;
	tziNew.StandardDate.wSecond = regTZI.StandardDate.wSecond;
	tziNew.StandardDate.wMilliseconds = regTZI.StandardDate.wMilliseconds;

	tziNew.DaylightDate.wYear = regTZI.DaylightDate.wYear;
	tziNew.DaylightDate.wMonth = regTZI.DaylightDate.wMonth;
	tziNew.DaylightDate.wDay = regTZI.DaylightDate.wDay;
	tziNew.DaylightDate.wDayOfWeek = regTZI.DaylightDate.wDayOfWeek;
	tziNew.DaylightDate.wHour = regTZI.DaylightDate.wHour;
	tziNew.DaylightDate.wMinute = regTZI.DaylightDate.wMinute;
	tziNew.DaylightDate.wSecond = regTZI.DaylightDate.wSecond;
	tziNew.DaylightDate.wMilliseconds = regTZI.DaylightDate.wMilliseconds;

	if (!SetDynamicTimeZoneInformation(&tziNew))
	{
		printf("STZI failed (%d)\n", GetLastError());
		return 0;
	}

	// Retrieve and display the newly set time zone information

	
	// Disable the privilege

	tkp.Privileges[0].Attributes = 0;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	printf("success~!");
	return 1;
}