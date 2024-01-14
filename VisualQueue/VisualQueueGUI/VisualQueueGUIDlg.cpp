
// VisualQueueGUIDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "VisualQueueGUI.h"
#include "VisualQueueGUIDlg.h"
#include "afxdialogex.h"
#include "QueueHelper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVisualQueueGUIDlg dialog



CVisualQueueGUIDlg::CVisualQueueGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VISUALQUEUEGUI_DIALOG, pParent)
	, m_NbrEltsGenerate(5)
	, m_nEnqueueValue(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	TRACE("\r\n****** Conctructeur CVisualQueueGUIDlg\r\n");
}

void CVisualQueueGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RAND_NBR_ELTS, m_NbrEltsGenerate);
	DDV_MinMaxInt(pDX, m_NbrEltsGenerate, 1, 10);
	DDX_Text(pDX, IDC_EDIT_ENQUEUE_VAL, m_nEnqueueValue);
	DDV_MinMaxInt(pDX, m_nEnqueueValue, 0, 99);
}

BEGIN_MESSAGE_MAP(CVisualQueueGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ENQUEUE, &CVisualQueueGUIDlg::OnClickedButtonEnqueue)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CVisualQueueGUIDlg::OnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_DEQUEUE, &CVisualQueueGUIDlg::OnClickedButtonDequeue)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &CVisualQueueGUIDlg::OnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_BUTTON_PEEK, &CVisualQueueGUIDlg::OnClickedButtonPeek)
END_MESSAGE_MAP()


// CVisualQueueGUIDlg message handlers

BOOL CVisualQueueGUIDlg::OnInitDialog()
{
	::srand(::time(NULL));
	m_nEnqueueValue = ::rand() % 100;

	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	_updateQueueContent();
	
	
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVisualQueueGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVisualQueueGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVisualQueueGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVisualQueueGUIDlg::OnClickedButtonEnqueue()
{
	if (UpdateData())
	{
		m_queueHelper.Enqueue(m_nEnqueueValue);
		_updateQueueContent();
		
		m_nEnqueueValue = ::rand() % 100;
		UpdateData(FALSE);
	}
}


void CVisualQueueGUIDlg::OnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	m_queueHelper.FreeQueue();
	_updateQueueContent();

}


void CVisualQueueGUIDlg::OnClickedButtonDequeue()
{
	m_queueHelper.Dequeue();	
	_updateQueueContent();
	CString strTemp;
	strTemp.Format(_T("&Défiler(%d)"), m_queueHelper.GetLastDequeuedValue());
	SetDlgItemText(IDC_BUTTON_DEQUEUE, strTemp);
}


void CVisualQueueGUIDlg::OnClickedButtonGenerate()
{
	
	if (UpdateData())
	{
		if (m_NbrEltsGenerate > 0)
		{
			m_queueHelper.InitQueue(m_NbrEltsGenerate);
			_updateQueueContent();
		}
	}

	

}


void CVisualQueueGUIDlg::OnClickedButtonPeek()
{
	// TODO: Add your control notification handler code here	
	m_queueHelper.Peek();
	_updateQueueContent();
	CString strTemp;
	strTemp.Format(_T("&Peek(%d)"), m_queueHelper.GetLastPeekedValue());
	SetDlgItemText(IDC_BUTTON_PEEK, strTemp);
	

}

void CVisualQueueGUIDlg::_updateQueueContent()
{
	SetDlgItemText(IDC_EDIT_QUEUE_CONTENT, m_queueHelper.GetTextRepresentation());
	CString strQueue;
	strQueue.Format(_T("&Queue(%d)"), m_queueHelper.GetQueueItemsCount());
	SetDlgItemText(IDC_LBL_QUEUE, strQueue);
	


}




