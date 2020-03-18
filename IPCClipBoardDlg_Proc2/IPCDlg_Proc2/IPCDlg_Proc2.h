
// IPCDlg_Proc2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CIPCDlgProc2App:
// See IPCDlg_Proc2.cpp for the implementation of this class
//

class CIPCDlgProc2App : public CWinApp
{
public:
	CIPCDlgProc2App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CIPCDlgProc2App theApp;
