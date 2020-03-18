#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<atlstr.h>
#include"tracerSA.h"


Tracer g_Tracer;

HWND  notepad;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int  WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	g_Tracer.Trace(L"~mycdipc~: In WinMain()");
	WNDCLASSEX wnd;
	TCHAR appName[] = { L"CopyData" };
	HWND hwnd;
	MSG msg;

	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = appName;
	wnd.lpszMenuName = NULL;
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.hCursor = LoadCursor(hInstance, L"IDI_CURSOR");
	wnd.hIcon = LoadIcon(hInstance, L"IDI_ICON");
	wnd.hIconSm = LoadIcon(hInstance, L"IDI_ICON");

	RegisterClassEx(&wnd);

	hwnd = CreateWindow(appName,
		L"ClipBoardViewer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hwnd == NULL)
	{
		MessageBox(hwnd, L"Create Window Failed", L"Error", MB_OK | MB_ICONERROR);
		g_Tracer.Trace(L"~mycdipc~: Create Window Failed Error:%d", GetLastError());
		return 0;
	}
	g_Tracer.Trace(L"~mycdipc~: Create Window succeeded");
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	

	while (!GetMessage(&msg, hwnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	g_Tracer.Trace(L"~mycdipc~: Out WinMain()");
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	g_Tracer.Trace(L"~mycdipc~: In WndProc()");

	switch (iMsg)
	{

	case WM_CREATE:
	{
		wchar_t sMsg[] = L"Hello this message is from Datacopy window to  WM_COPYDATA";
		COPYDATASTRUCT data;
		data.dwData = 1;
		data.cbData = 39 * sizeof(wchar_t);
		data.lpData = &sMsg;
		
		SendMessage(hwnd, WM_COPYDATA, (WPARAM)(HWND)notepad, (LPARAM)(LPVOID)&data);
		g_Tracer.Trace(L"~mycdipc~: Send Message to Notepad succeeded");

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_COPYDATA:
	{
		ShellExecute(NULL, L"open", L"notepad.exe", NULL, 0, SW_SHOW);
		g_Tracer.Trace(L"~mycdipc~: Open Notepad Window succeeded");
		Sleep(3000);
		PCOPYDATASTRUCT pcData;
		pcData = (PCOPYDATASTRUCT)lParam;
		CString* sRecvMsg = (CString*)pcData->lpData;
		notepad = FindWindow(TEXT("Notepad"), NULL);

		if ((notepad == INVALID_HANDLE_VALUE) || (notepad == NULL))
		{
			PostQuitMessage(0);
			break;
		}
	    notepad = FindWindowEx(notepad, NULL, TEXT("edit"), NULL);
		
		SendMessage(notepad, WM_SETTEXT, 0,(LPARAM)sRecvMsg);
		Sleep(3000);
		g_Tracer.Trace(L"~mycdipc~: Copy data in Notepad succeeded");

		break;
	}
	}
	g_Tracer.Trace(L"~mycdipc~: Out WndProc()");

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

