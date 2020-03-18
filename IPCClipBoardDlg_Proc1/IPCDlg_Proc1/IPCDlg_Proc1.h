
// IPCDlg_Proc1.h : main header file for the PROJECT_NAME application
//
#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include<string>

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CIPCDlgProc1App:
// See IPCDlg_Proc1.cpp for the implementation of this class
//

class CIPCDlgProc1App : public CWinApp
{
public:
	CIPCDlgProc1App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CIPCDlgProc1App theApp;
