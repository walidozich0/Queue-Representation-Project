
// VisualQueueGUIDlg.h : header file
//

#pragma once


#include "QueueHelper.h"


// CVisualQueueGUIDlg dialog
class CVisualQueueGUIDlg : public CDialogEx
{
// Construction
public:
	CVisualQueueGUIDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VISUALQUEUEGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonEnqueue();
	afx_msg void OnClickedButtonClear();
	afx_msg void OnClickedButtonDequeue();
	afx_msg void OnClickedButtonGenerate();
	afx_msg void OnClickedButtonPeek();
	int m_NbrEltsGenerate;
	int m_nEnqueueValue;
private:
	CQueueHelper  m_queueHelper;
	void _updateQueueContent();

	
};
