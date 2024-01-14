#pragma once
#include "QueueHelper.h"


// CVisualQueueDrawerCtrl

class CVisualQueueDrawerCtrl : public CStatic
{
	DECLARE_DYNAMIC(CVisualQueueDrawerCtrl)

public:
	COLORREF m_clrBackground;
	CQueueDrawHelper* m_pQueueDrawHelper;

	CVisualQueueDrawerCtrl();
	virtual ~CVisualQueueDrawerCtrl();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType,	int cx,	int cy);
	

protected:
	DECLARE_MESSAGE_MAP()
};


