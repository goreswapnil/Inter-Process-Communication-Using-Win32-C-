
// IPCDlg_Proc1Dlg.h : header file
//
#define _CRT_SECURE_NO_WARNINGS 
#pragma once



// CIPCDlgProc1Dlg dialog
class CIPCDlgProc1Dlg : public CDialogEx
{
// Construction
public:
	CIPCDlgProc1Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPCDLG_PROC1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HANDLE m_hEvent_1, m_hEvent_2,  m_hThread;
	
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void ChkEvent();
	afx_msg void OnEnChangeClipReci();
	afx_msg void OnEnChangeClipSend();
	afx_msg void OnBnClickedClipButton();
	
	CEdit m_edtToClipboard;
	CString Edit_Receive;
	CEdit m_edt1;
};
