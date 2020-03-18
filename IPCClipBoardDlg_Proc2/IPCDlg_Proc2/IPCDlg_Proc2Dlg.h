
// IPCDlg_Proc2Dlg.h : header file
//

#pragma once


// CIPCDlgProc2Dlg dialog
class CIPCDlgProc2Dlg : public CDialogEx
{
// Construction
public:
	CIPCDlgProc2Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPCDLG_PROC2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HANDLE m_hEvent_1, m_hEvent_2, m_hThread;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void ChkEvent();
	afx_msg void OnEnChangeEditRecv();
	afx_msg void OnEnChangeEditSend();
	afx_msg void OnBnClickedButtonSend();
	CEdit m_edtToClipboard;
	CEdit m_edt1;
	CString Edit_Receive;
};
