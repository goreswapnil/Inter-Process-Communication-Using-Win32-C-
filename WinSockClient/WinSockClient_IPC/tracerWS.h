#pragma once
#include<Windows.h>
#include <string>

using namespace std;

#define MSG_BUFFER_SIZE		2048
#define TIME_STAMP_SIZE		100

class Tracer
{
private:
	CRITICAL_SECTION	m_cs;
	HANDLE				m_hLogFile;
	WCHAR				m_wstrTimeStamp[TIME_STAMP_SIZE];
	WCHAR				m_wstrArgBuffer[MSG_BUFFER_SIZE];
	DWORD				m_dwPID;
	SYSTEMTIME			m_sysTime;

public:

	Tracer()
	{
		m_hLogFile = INVALID_HANDLE_VALUE;
		m_dwPID = GetCurrentProcessId();

		InitializeCriticalSection(&m_cs);

		m_hLogFile = CreateFileW(L"D:\\MyWinSockClient.Log", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0);

		if (m_hLogFile != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(m_hLogFile, 0, 0, SEEK_SET);
			wchar_t BOM = 0xfeff;
			DWORD dwWritten = 0;
			WriteFile(m_hLogFile, &BOM, sizeof(wchar_t), &dwWritten, NULL);
			SetFilePointer(m_hLogFile, 0, 0, SEEK_END);
		}

	}

	~Tracer()
	{
		if (m_hLogFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hLogFile);
			m_hLogFile = INVALID_HANDLE_VALUE;
		}
		DeleteCriticalSection(&m_cs);
	}

	void Trace(LPCWSTR argv, ...)
	{
		if (m_hLogFile == INVALID_HANDLE_VALUE)
			return;

		EnterCriticalSection(&m_cs);

		try
		{
			wstring wstrDbgMsg = L"\r\n";
			va_list argptr = NULL;
			DWORD dwWritten = 0;

			SecureZeroMemory(m_wstrArgBuffer, sizeof(m_wstrArgBuffer));
			SecureZeroMemory(m_wstrTimeStamp, sizeof(m_wstrTimeStamp));
			SecureZeroMemory(&m_sysTime, sizeof(m_sysTime));

			va_start(argptr, argv);
			_vsnwprintf_s(m_wstrArgBuffer, MSG_BUFFER_SIZE, _TRUNCATE, argv, argptr);
			va_end(argptr);

			GetLocalTime(&m_sysTime);
			wsprintfW(m_wstrTimeStamp, L"[%02d/%02d/%d %02d:%02d:%02d:%03d][P%04d,T%04d] ", m_sysTime.wDay, m_sysTime.wMonth, m_sysTime.wYear, m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMilliseconds, m_dwPID, GetCurrentThreadId());

			wstrDbgMsg += m_wstrTimeStamp;
			wstrDbgMsg += m_wstrArgBuffer;

			WriteFile(m_hLogFile, wstrDbgMsg.c_str(), wstrDbgMsg.length() * sizeof(wchar_t), &dwWritten, NULL);

			OutputDebugStringW(wstrDbgMsg.c_str());
		}
		catch (...)
		{
			//do nothing
		}

		LeaveCriticalSection(&m_cs);
	}
};