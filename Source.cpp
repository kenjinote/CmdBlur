#include<windows.h>

BOOL bSetWindowBlur;

void SetWindowBlur(HWND hWnd)
{
	const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
	if (hModule)
	{
		struct ACCENTPOLICY
		{
			int nAccentState;
			int nFlags;
			int nColor;
			int nAnimationId;
		};
		struct WINCOMPATTRDATA
		{
			int nAttribute;
			PVOID pData;
			ULONG ulDataSize;
		};
		typedef BOOL(WINAPI*pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
		const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
		if (SetWindowCompositionAttribute)
		{
			ACCENTPOLICY policy = { 3, 0, 0, 0 };
			WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) };
			SetWindowCompositionAttribute(hWnd, &data);
		}
		FreeLibrary(hModule);
	}
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	if (dwProcessId == lParam)
	{
		SetWindowBlur(hWnd);
		bSetWindowBlur = TRUE;
		return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	PROCESS_INFORMATION pInfo = { 0 };
	STARTUPINFO sInfo = { 0 };

	sInfo.cb = sizeof(STARTUPINFO);
	sInfo.dwFlags = STARTF_USESHOWWINDOW;
	sInfo.wShowWindow = nCmdShow;

	TCHAR szCommandLine[MAX_PATH];
	if (GetEnvironmentVariable(TEXT("ComSpec"), szCommandLine, MAX_PATH) && CreateProcess(0, szCommandLine, 0, 0, 0, 0, 0, 0, &sInfo, &pInfo))
	{
		CloseHandle(pInfo.hThread);
		WaitForInputIdle(pInfo.hProcess, INFINITE);
		CloseHandle(pInfo.hProcess);
		while (!bSetWindowBlur)
		{
			EnumWindows(EnumWindowsProc, (LPARAM)pInfo.dwProcessId);
		}
	}
	return 0;
}
