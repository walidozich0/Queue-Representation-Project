// VisualQueueDrawerCtrl.cpp : implementation file
//

#include "pch.h"
#include "VisualQueueGUI.h"
#include "VisualQueueDrawerCtrl.h"


// CVisualQueueDrawerCtrl

IMPLEMENT_DYNAMIC(CVisualQueueDrawerCtrl, CStatic)

CVisualQueueDrawerCtrl::CVisualQueueDrawerCtrl()
{
	m_clrBackground = RGB(255, 255, 255);
}

CVisualQueueDrawerCtrl::~CVisualQueueDrawerCtrl()
{
}

void CVisualQueueDrawerCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	

	if (m_pQueueDrawHelper)
	{
		m_pQueueDrawHelper->Draw();
	}
}

BOOL CVisualQueueDrawerCtrl::OnEraseBkgnd(CDC* pDC)
{
	//return CStatic::OnEraseBkgnd(pDC);
	return FALSE;
}

void CVisualQueueDrawerCtrl::OnSize(UINT nType, int cx, int cy)
{
	if (m_pQueueDrawHelper) m_pQueueDrawHelper->OnResizeWindow();
}


BEGIN_MESSAGE_MAP(CVisualQueueDrawerCtrl, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

	




// CVisualQueueDrawerCtrl message handlers


