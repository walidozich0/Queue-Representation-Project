
// VisualQueueGUIDlg.h : header file
//

#pragma once


#include "QueueHelper.h"
#include "VisualQueueDrawerCtrl.h"


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
	void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg
	void OnCancel();
	void OnClickedButtonEnqueue();
	afx_msg void OnClickedButtonClear();
	afx_msg void OnClickedButtonDequeue();
	afx_msg void OnClickedButtonGenerate();
	afx_msg void OnClickedButtonPeek();
	int m_NbrEltsGenerate;
	int m_nEnqueueValue;
private:
	CQueueHelper  m_queueHelper;
	CQueueDrawHelper m_queueDrawHelper;
	void _updateQueueContent();

	
public:
	CVisualQueueDrawerCtrl m_wndDrawCtrl;
	afx_msg void OnBnClickedBtnPauseResume();
	BOOL m_bAnimationEnabled;
};
