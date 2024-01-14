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
	;

	if (m_pQueueDrawHelper)
	{
		m_pQueueDrawHelper->Draw(&dc, &CRect(lpDrawItemStruct->rcItem));
	}

}


BEGIN_MESSAGE_MAP(CVisualQueueDrawerCtrl, CStatic)

END_MESSAGE_MAP()

	




// CVisualQueueDrawerCtrl message handlers


