#include "pch.h"
#include "QueueHelper.h"



CQueueHelper::CQueueHelper():
	m_pQueue(NULL)
	, m_strTextRepresentation(_T("(File Non Initialisée)"))
	, m_nLastEnqueuedValue(-1)
	, m_nLastDequeuedValue(-1)
	, m_nLastPeekedValue(-1)
	, m_nMaxItems(15)
{
	TRACE("\r\n******* Constructeur CQueueHelper appelé\r\n");
	
}

CQueueHelper::~CQueueHelper()
{
	TRACE("\r\n******* Destructeur CQueueHelper appelé\r\n");
	FreeQueue();
}


BOOL CQueueHelper::IsQueueInitialized()
{
	return m_pQueue!=NULL;
}

void CQueueHelper::InitQueue(int nItemsCount)
{
	FreeQueue();
	_initFileIfNot();
	if (nItemsCount > 0)
	{		
		
		::Queue_RandFill(m_pQueue, nItemsCount);
		_generateTextData();
	}

}

void CQueueHelper::Enqueue(int nValue)
{
	_initFileIfNot();
	
	if (GetQueueItemsCount() >= m_nMaxItems)
	{
		AfxMessageBox(_T("Vous avez atteint le max d'elements"),MB_ICONEXCLAMATION);
		return;
	}
	
	::Enqueue(m_pQueue, nValue);
	m_nLastEnqueuedValue = nValue;
	_generateTextData();// regenerer la represenattion textuelle
}

int CQueueHelper::Peek()
{
	if (!IsQueueInitialized()) return -1;
	m_nLastPeekedValue = ::peek(m_pQueue);
}

int CQueueHelper::Dequeue()
{
	if (!IsQueueInitialized()) return -1;
	m_nLastDequeuedValue = ::Dequeue(m_pQueue);
	_generateTextData();// regenerer la represenattion textuelle
}

int CQueueHelper::GetQueueItemsCount()
{
	if (!IsQueueInitialized()) return 0;
	return m_pQueue->m_nItemsCount;
}

void CQueueHelper::FreeQueue()
{
	if (m_pQueue)
	{
		::Queue_EmptyFree(m_pQueue);
		::free(m_pQueue);
		m_pQueue = NULL;
	}
	m_strTextRepresentation = _T("(File Non Initialisée)");
	m_nLastEnqueuedValue = m_nLastDequeuedValue = m_nLastPeekedValue = -1;
}

void CQueueHelper::_initFileIfNot()
{
	if (!m_pQueue)
	{
		m_pQueue = QueueInit();
		m_strTextRepresentation = _T("(File Vide)");
		m_nLastEnqueuedValue = m_nLastDequeuedValue = m_nLastPeekedValue = -1;
	}
}

void CQueueHelper::_generateTextData()
{
	if (!IsQueueInitialized())
	{
		m_strTextRepresentation = _T("(File Non Initialisée)");
		m_strTextRepresentationEx = _T("");
	}
	else if (GetQueueItemsCount() == 0)
	{
		m_strTextRepresentation = _T("(File existe mais Vide)");
		m_strTextRepresentationEx = _T("");
	}
	else
	{
		m_strTextRepresentation = _T("Head");
		m_strTextRepresentationEx = _T("");

		QueueEntry* pEntry = m_pQueue->m_pHeadEntry;
		while (pEntry)
		{
			CString strTemp;
			strTemp.Format(_T("->(%02d)"), pEntry->m_nData);
			m_strTextRepresentation += strTemp;

			if(pEntry->m_pNextEntry)
				strTemp.Format(_T("%04X%02d%04X"),(WORD)pEntry, pEntry->m_nData, (WORD)(pEntry->m_pNextEntry));
			else
				strTemp.Format(_T("%04X%02d0000"), (WORD)pEntry, pEntry->m_nData);
			m_strTextRepresentationEx += strTemp;
			pEntry = pEntry->m_pNextEntry;
		}

		m_strTextRepresentation += _T("<-Tail");
	}
}


///////////////////////////////////////
// CQueueDrawSettings



CQueueDrawHelper::CQueueDrawHelper():
	m_pQueueHelper(NULL), m_pDrawingWindow(NULL)
{
	//int nFactor = 10;

	m_nMarge = 2; 
	m_nTopY = 8;
	m_nBottomY = 8; 
	m_nDataWidth = 4;
	m_nDataHeight = 4; 
	m_nDataDistanceFromQueue = 16;

	m_nElementWidth = 8;
	m_nElementHeight = 16;
	m_nElementAdrHeight = 4; 
	m_DrawUnitLength = CSize(0, 0);
	
}

void CQueueDrawHelper::Draw(CDC* pDC, CRect* pDrawCtrlRect)
{
	//Graphics graphics(pDC->GetSafeHdc());
	
	CMemDC memDC(*pDC, *pDrawCtrlRect);
	Graphics graphics(memDC.GetDC());
	
	

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	//graphics.SetSmoothingMode(SmoothingModeHighQuality);
	//graphics.SetSmoothingMode(SmoothingModeHighSpeed);
	

	// temp

	FontFamily   fontFamily(L"Tahoma");
	Gdiplus:: Font font(&fontFamily, 12, FontStyleRegular, UnitPoint);
	SolidBrush   solidBrush(Color(255, 0, 0, 0));

	
	

	SolidBrush backgroundBrush(Color(255,255,255,255));
	SolidBrush backgroundBrushElt(Color(255, 153, 204, 255));
	SolidBrush backgroundBrushAdr(Color(255, 204, 255, 204));
	SolidBrush backgroundBrushAdrSuiv(Color(255, 255, 255, 204));
	SolidBrush backgroundBrushData(Color(255, 204, 229, 255));
	SolidBrush backgroundBrushEnqued(Color(150, 153, 153, 255));
	SolidBrush backgroundBrushDequed(Color(255, 255, 178, 102));
	SolidBrush backgroundBrushPeeked(Color(255, 255, 204, 229));
	
	Pen normalPen(Color(255, 0, 0, 0), 2.0F);
	Pen DataPen(Color(255, 0, 0, 0), 1.5F);

	// dashed pen
	REAL dashValues[2] = { 1, 1 };
	Pen dashedPen(Color(255, 192, 192, 192), 2.0F);
	dashedPen.SetDashPattern(dashValues, 2);

	REAL dashValues2[2] = { 1, 1 };
	Pen dashedPen2(Color(255, 192, 192, 192), 1.5F);
	dashedPen2.SetDashPattern(dashValues2, 2);

	// arrow pen
	Pen arrowPen(Color(255, 128, 0, 0), 2.0F);
	Pen BlackArrowPen(Color(255, 0, 0, 0), 2.0F);
	AdjustableArrowCap myArrow(5, 5, true);
	myArrow.SetMiddleInset(2.0f);
	arrowPen.SetCustomEndCap(&myArrow);
	BlackArrowPen.SetCustomEndCap(&myArrow);
	

	graphics.FillRectangle(&backgroundBrush, 0, 0, pDrawCtrlRect->Width(), pDrawCtrlRect->Height());
	
	
	// dequeue zone
	{

		CString strDequedValue;
		strDequedValue.Format(_T("%02d"), m_pQueueHelper->GetLastDequeuedValue());
		CRect rcLogical = ComputeDequeueDataPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillEllipse(&backgroundBrushDequed, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawEllipse(&DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(strDequedValue, -1, &font,
		RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);
	}

	{
		CRect rcLogical = ComputeDequeuedElementZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.DrawRectangle(&dashedPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
	}

	{
		CRect rcLogical = ComputeQueueZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		//graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());		
		graphics.DrawLine(&normalPen, rcPhysical.left, rcPhysical.top,rcPhysical.right, rcPhysical.top);
		graphics.DrawLine(&normalPen, rcPhysical.left, rcPhysical.bottom, rcPhysical.right, rcPhysical.bottom);
	}

	{
		CRect rcLogical = ComputeEnqueuedElementZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.DrawRectangle(&dashedPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		
	}

	{
		CRect rcLogical = ComputeQueueExtraZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		//graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());		
		graphics.DrawLine(&dashedPen2, rcPhysical.left, rcPhysical.top, rcPhysical.right, rcPhysical.top);
		graphics.DrawLine(&dashedPen2, rcPhysical.left, rcPhysical.bottom, rcPhysical.right, rcPhysical.bottom);
	}

	{
		CString strPeekedValue;
		strPeekedValue.Format(_T("%02d"), m_pQueueHelper->GetLastPeekedValue());
		CRect rcLogical = ComputePeekedDataPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillEllipse(&backgroundBrushPeeked, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawEllipse(&DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(strPeekedValue, -1, &font,
		RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);
	}

	
	{
		CString strEnquedValue;
		strEnquedValue.Format(_T("%02d"), m_pQueueHelper->GetLastEnqueuedValue());
		CRect rcLogical = ComputeEnqueuedDataPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillEllipse(&backgroundBrushData, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawEllipse(&DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(strEnquedValue, -1, &font,
		RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);
	}

	//for (int i = 0; i < m_pQueueHelper->GetQueueItemsCount(); i++)
	//{
	//	CString strCurrentData = m_pQueueHelper->GetTextRepresentationEx().Mid(i * 10, 10);
	//	CString strTemp = strCurrentData.Mid(4, 2);
	//	AfxMessageBox(strTemp);
	//}
	
	// queue zone
	for (int i = 0; i < m_pQueueHelper->GetQueueItemsCount(); i++)
	{
		CString strCurrentData = m_pQueueHelper->GetTextRepresentationEx().Mid(i * 10, 10);
		
		ASSERT(strCurrentData.GetLength() == 10);

		{
			CRect rcLogical = ComputeQueueElementPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			graphics.FillRectangle(&backgroundBrushElt, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			}
		{
			CRect rcLogical = ComputeQueueElementAdrPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			graphics.FillRectangle(&backgroundBrushAdr, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawString(strCurrentData.Left(4), -1, &font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);

		}
		{
			CRect rcLogical = ComputeQueueElementDataPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			graphics.FillEllipse(&backgroundBrushData, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawEllipse(&DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawString(strCurrentData.Mid(4,2), -1, &font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);
			


		}
		{
			CRect rcLogical = ComputeQueueElementNextAdrPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			CString strCompareZero = _T("0000");

			if (strCurrentData.Right(4)==strCompareZero)
			{
				graphics.FillRectangle(&backgroundBrushAdrSuiv, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawLine(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.right, rcPhysical.bottom);
			}
			else {
				graphics.FillRectangle(&backgroundBrushAdrSuiv, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawString(strCurrentData.Right(4), -1, &font,
				RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);
			}
			
			
			}

		if (i != (m_pQueueHelper->GetQueueItemsCount()-1))
		{
			

			CPoint ptStartLogical = ComputeQueueElementOutArrowPos(i);
			CPoint ptEndLogical = ComputeQueueElementInArrowPosFromPrevElt(i + 1);

			CPoint ptInterm1Logical = ptStartLogical;
			ptInterm1Logical.Offset((ptEndLogical.x- ptStartLogical.x)/2, 0);
			CPoint ptInterm2Logical = ptEndLogical;
			ptInterm2Logical.Offset(-(ptEndLogical.x - ptStartLogical.x)/2, 0);



			CPoint ptStartPhysical = _logicalUnits2DeviceUnits(&ptStartLogical);
			CPoint ptEndPhysical = _logicalUnits2DeviceUnits(&ptEndLogical);
			CPoint ptInterm1Physical = _logicalUnits2DeviceUnits(&ptInterm1Logical);
			CPoint ptInterm2Physical = _logicalUnits2DeviceUnits(&ptInterm2Logical);

			//graphics.DrawLine(&arrowPen, ptStartPhysical.x, ptStartPhysical.y, ptEndPhysical.x, ptEndPhysical.y);
			
			Point pointsColl[4] = { Point(),Point(),Point(),Point() };
			pointsColl[0].X = ptStartPhysical.x; pointsColl[0].Y = ptStartPhysical.y;
			pointsColl[1].X = ptInterm1Physical.x; pointsColl[1].Y = ptInterm1Physical.y;
			pointsColl[2].X = ptInterm2Physical.x; pointsColl[2].Y = ptInterm2Physical.y;
			pointsColl[3].X = ptEndPhysical.x; pointsColl[3].Y = ptEndPhysical.y;

			//graphics.DrawLines(&arrowPen, pointsColl,4);
			graphics.DrawCurve(&arrowPen, pointsColl, 4);
			
			//graphics.DrawBeziers(&arrowPen, pointsColl, 4);

		}

	}


	{
		CRect rcLogical = ComputeHeadPointerPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillRectangle(&backgroundBrushAdr, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(_T("Head"), -1, &font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);
	}

	{
		if (m_pQueueHelper->GetQueueItemsCount() > 0)
		{
			CPoint ptStartLogical = ComputeHeadPointerOutArrowPos();			
			CPoint ptEndLogical = ComputeQueueElementInArrowPosFromHeader(0);
			CPoint ptStartPhysical = _logicalUnits2DeviceUnits(&ptStartLogical);
			CPoint ptEndPhysical = _logicalUnits2DeviceUnits(&ptEndLogical);
			
			graphics.DrawLine(&BlackArrowPen, ptStartPhysical.x, ptStartPhysical.y, ptEndPhysical.x, ptEndPhysical.y);
		}
	}



	{
		CRect rcLogical = ComputeTailPointerPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillRectangle(&backgroundBrushAdr, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(_T("Tail"), -1, &font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &solidBrush);
	}

	{
		if (m_pQueueHelper->GetQueueItemsCount() > 0)
		{
			CPoint ptStartLogical = ComputeTailPointerOutArrowPos();
			CPoint ptStartPhysical = _logicalUnits2DeviceUnits(&ptStartLogical);
			CPoint ptEndLogical = ComputeQueueElementInArrowPosFromTail(m_pQueueHelper->GetQueueItemsCount() - 1);
			CPoint ptEndPhysical = _logicalUnits2DeviceUnits(&ptEndLogical);
			graphics.DrawLine(&BlackArrowPen, ptStartPhysical.x, ptStartPhysical.y, ptEndPhysical.x, ptEndPhysical.y);
		}
	}


	

	
	

}

void CQueueDrawHelper::OnInit(CQueueHelper* pQH, CWnd* pWnd)
{
	ASSERT(pQH);
	ASSERT(pWnd && pWnd->GetSafeHwnd());
	
	m_pQueueHelper = pQH;
	m_pDrawingWindow = pWnd;

	m_rcDrawingWindowLogicalPosAndSize = computeCanvasRect();
	m_pDrawingWindow->GetWindowRect(&m_rcDrawingWindowPhysicalPosAndSize);
	_computeUnitsConvParams();
}

void CQueueDrawHelper::OnResizeWindow()
{
	ASSERT(m_pDrawingWindow && m_pDrawingWindow->GetSafeHwnd());
	m_pDrawingWindow->GetWindowRect(&m_rcDrawingWindowPhysicalPosAndSize);
	_computeUnitsConvParams();

}

CRect CQueueDrawHelper::computeCanvasRect()
{
	ASSERT(m_pQueueHelper);

	return CRect(CPoint(0,0),CSize(m_nMarge + (m_nElementWidth+ 2* m_nMarge) * (1 + m_pQueueHelper->GetQueueMaxItemsCount()) + m_nMarge,
		m_nTopY + m_nDataHeight + m_nDataDistanceFromQueue + (2*m_nMarge + m_nDataHeight) + m_nDataDistanceFromQueue + m_nDataHeight 
		+ m_nBottomY));
}

CRect CQueueDrawHelper::ComputeDequeueDataPos()
{
	//(x= marge*3 ; y= TopMarge)	
	return CRect(
		CPoint(3 * m_nMarge, m_nTopY),
		CSize(m_nDataWidth, m_nDataHeight));
}

CRect CQueueDrawHelper::ComputeDequeuedElementZone()//2	
{
	//(x= marge ; y= TopMarge+Dh+DistanceDQ )
	return CRect(
		CPoint(m_nMarge, m_nTopY + m_nDataHeight + m_nDataDistanceFromQueue),
		CSize(2 * m_nMarge + m_nElementWidth, 2 * m_nMarge + m_nElementHeight));
	
}

CRect CQueueDrawHelper::ComputeDequeuedElementPos()
{
	CRect rcDequeuedElementZone = ComputeDequeuedElementZone();
	// -3: (x = X(2) + marge; y = y(2) + marge)
	return CRect(CPoint(rcDequeuedElementZone.left + m_nMarge,rcDequeuedElementZone.top + m_nMarge),
		CSize(m_nElementWidth,m_nElementHeight));
}

CRect CQueueDrawHelper::ComputeDequeuedElementAdrPos()
{
	CRect rcDequeuedElementZone = ComputeDequeuedElementZone();
	// -3: (x = X(2) + marge; y = y(2) + marge)
	return CRect(CPoint(rcDequeuedElementZone.left + m_nMarge, rcDequeuedElementZone.top + m_nMarge),
		CSize(m_nElementWidth, m_nElementAdrHeight));
}

CRect CQueueDrawHelper::ComputeDequeuedElementDataPos()
{
	// -  4: (x= X(3)+marge ; y= y(3)+@h+marge)
	CRect rcDequeuedElementZone = ComputeDequeuedElementZone();

	return CRect(CPoint(rcDequeuedElementZone.left + m_nMarge,rcDequeuedElementZone.top+m_nElementAdrHeight+m_nMarge),
		CSize(m_nDataWidth,m_nDataHeight));
}

CRect CQueueDrawHelper::ComputeDequeuedElementNextAdrPos()
{
	//-  5: (x= X(3) ;y= y(3)+ EltH - @h)
	CRect rcDequeuedElementZone = ComputeDequeuedElementZone();

	return CRect(CPoint(rcDequeuedElementZone.left , rcDequeuedElementZone.top + m_nElementHeight - m_nElementAdrHeight ),
		CSize(m_nElementWidth, m_nElementAdrHeight));
}

CRect CQueueDrawHelper::ComputeQueueZone()
{
	ASSERT(m_pQueueHelper);//!! doit être initilaisée
	//-  6: (x= marge*3+EltW ; y= TopMarge+Dh+DistanceDQ )

	return CRect(CPoint(m_nElementWidth + 3 * m_nMarge, m_nTopY + m_nDataHeight + m_nDataDistanceFromQueue),
		CSize(m_pQueueHelper->GetQueueItemsCount()* (m_nElementWidth + 2* m_nMarge), m_nElementHeight + 2 * m_nMarge));
}

CRect CQueueDrawHelper::ComputeQueueExtraZone()
{
	ASSERT(m_pQueueHelper);
	CRect rc = ComputeEnqueuedElementZone();
	int nRemainingItems = m_pQueueHelper->GetQueueMaxItemsCount() - 1 /*enqueuezone*/ - m_pQueueHelper->GetQueueItemsCount();

	return (nRemainingItems <= 0) ? 
	CRect(CPoint(rc.right,rc.top), CSize(0,rc.Height()))
	:
	CRect(CPoint(rc.right, rc.top), CSize((m_nElementWidth + 2 * m_nMarge)* nRemainingItems,m_nElementHeight + 2 * m_nMarge));	
}

void CQueueDrawHelper::_computeUnitsConvParams()
{
	
	m_DrawUnitLength.cx = (m_rcDrawingWindowPhysicalPosAndSize.Size().cx) / (m_rcDrawingWindowLogicalPosAndSize.Width());
	m_DrawUnitLength.cy = (m_rcDrawingWindowPhysicalPosAndSize.Size().cy) / (m_rcDrawingWindowLogicalPosAndSize.Height());

	if (m_DrawUnitLength.cx < m_DrawUnitLength.cy) m_DrawUnitLength.cy = m_DrawUnitLength.cx;
	if (m_DrawUnitLength.cx > m_DrawUnitLength.cy) m_DrawUnitLength.cx = m_DrawUnitLength.cy;

}

RECT CQueueDrawHelper::_convert(CRect* pRC)
{
	RECT r;
	r.left = pRC->left;
	r.right = pRC->right;
	r.top = pRC->top;
	r.bottom = pRC->bottom;
	return r;
}

CRect CQueueDrawHelper::_logicalUnits2DeviceUnits(CRect* pRC)
{
	CRect rc(pRC);
	rc.left *= m_DrawUnitLength.cx;
	rc.right *= m_DrawUnitLength.cx;
	rc.top *= m_DrawUnitLength.cy;
	rc.bottom *= m_DrawUnitLength.cy;
	return rc;
}

CPoint CQueueDrawHelper::_logicalUnits2DeviceUnits(CPoint* pPt)
{
	CPoint pt(pPt->x,pPt->y);
	pt.x *= m_DrawUnitLength.cx;
	pt.y *= m_DrawUnitLength.cy;	
	return pt;
}


CRect CQueueDrawHelper::ComputeHeadElementPos()
{
	CRect rcQueueZone = ComputeQueueZone();
	// -8: (x = X(6) + marge; y = y(6) + marge)
	return CRect(CPoint(rcQueueZone.left + m_nMarge, rcQueueZone.top + m_nMarge),
		CSize(m_nElementWidth, m_nElementHeight));
}

CRect CQueueDrawHelper::ComputeHeadElementAdrPos()
{
	CRect rcHeadElementZone = ComputeHeadElementPos();
	// -8: (x = X(6) + marge; y = y(6) + marge)
	return CRect(CPoint(rcHeadElementZone.left, rcHeadElementZone.top),
		CSize(m_nElementWidth, m_nElementAdrHeight));
}

CRect CQueueDrawHelper::ComputeHeadElementDataPos()
{
	//-9: (x = X(8) + marge; y = y = y(8) + @h + marge)
	CRect rcHeadElementZone = ComputeHeadElementPos();

	return CRect(CPoint(rcHeadElementZone.left + m_nMarge, rcHeadElementZone.top + m_nElementAdrHeight + m_nMarge),
		CSize(m_nDataWidth, m_nDataHeight));
}

CRect CQueueDrawHelper::ComputeHeadElementNextAdrPos()
{
	
	// - 10: (x= X(8) ; y= y(8)+ EltH - @h )
	CRect rcHeadElementZone = ComputeHeadElementPos();

	return CRect(CPoint(rcHeadElementZone.left, rcHeadElementZone.top + m_nElementHeight - m_nElementAdrHeight),
		CSize(m_nElementWidth, m_nElementAdrHeight));
}

CRect CQueueDrawHelper::ComputeQueueElementPos(int index)
{
	ASSERT(m_pQueueHelper);
	ASSERT(index >= 0 && index < m_pQueueHelper->GetQueueItemsCount());

	if (index == 0) return ComputeHeadElementPos();
	else
	{
		CRect rcPrevElement = ComputeQueueElementPos(index - 1);		
		return CRect(CPoint(rcPrevElement.right + 2 * m_nMarge,rcPrevElement.top),CSize(rcPrevElement.Size()));
	}
	
}

CRect CQueueDrawHelper::ComputeQueueElementAdrPos(int index)
{
	CRect rcElementZone = ComputeQueueElementPos(index);
	// -8: (x = X(6) + marge; y = y(6) + marge)
	return CRect(CPoint(rcElementZone.left, rcElementZone.top),
		CSize(m_nElementWidth, m_nElementAdrHeight));
}

CRect CQueueDrawHelper::ComputeQueueElementDataPos(int index)
{
	//-9: (x = X(8) + marge; y = y = y(8) + @h + marge)
	CRect rcElementZone = ComputeQueueElementPos(index);

	return CRect(CPoint(rcElementZone.left + m_nMarge, rcElementZone.top + m_nElementAdrHeight + m_nMarge),
		CSize(m_nDataWidth, m_nDataHeight));
}

CRect CQueueDrawHelper::ComputeQueueElementNextAdrPos(int index)
{
	CRect rcElementZone = ComputeQueueElementPos(index);

	return CRect(CPoint(rcElementZone.left, rcElementZone.top + m_nElementHeight - m_nElementAdrHeight),
		CSize(m_nElementWidth, m_nElementAdrHeight));
}

CPoint CQueueDrawHelper::ComputeQueueElementInArrowPosFromHeader(int index)
{
	CRect rcElement = ComputeQueueElementPos(index);
	return CPoint(rcElement.left + rcElement.Width()/2, rcElement.top);
}

CPoint CQueueDrawHelper::ComputeQueueElementInArrowPosFromTail(int index)
{
	CRect rcElement = ComputeQueueElementPos(index);
	return CPoint(rcElement.left + rcElement.Width() / 2, rcElement.bottom);
}

CPoint CQueueDrawHelper::ComputeQueueElementInArrowPosFromPrevElt(int index)
{
	CRect rcElement = ComputeQueueElementAdrPos(index);
	return CPoint(rcElement.left, rcElement.top+ rcElement.Height()/2);
}

CPoint CQueueDrawHelper::ComputeQueueElementOutArrowPos(int index)
{
	CRect rcElement = ComputeQueueElementNextAdrPos(index);
	return CPoint(rcElement.right, rcElement.top + (rcElement.Height())/2);
}

CRect CQueueDrawHelper::ComputeEnqueuedElementZone()
{
	ASSERT(m_pQueueHelper);

	//(x= marge ; y= TopMarge+Dh+DistanceDQ ) ComputeQueueZone()
	CRect rcQueueZone = ComputeQueueZone();	
	return CRect(
		CPoint(rcQueueZone.right, rcQueueZone.top),
		CSize(
			(m_pQueueHelper->GetQueueItemsCount() < m_pQueueHelper->GetQueueMaxItemsCount()) ? 
			(2 * m_nMarge + m_nElementWidth) : 0
			, 2 * m_nMarge + m_nElementHeight));

}

CRect CQueueDrawHelper::ComputeEnqueuedElementPos()
{
	CRect rcEnqueuedElementZone = ComputeEnqueuedElementZone();

	if (rcEnqueuedElementZone.Width() == 0)	return CRect();	 
	
	return CRect(CPoint(rcEnqueuedElementZone.left + m_nMarge, rcEnqueuedElementZone.top + m_nMarge),
		CSize(m_nElementWidth, m_nElementHeight));
}

CRect CQueueDrawHelper::ComputeEnqueuedElementAdrPos()
{
	CRect rcElementZone = ComputeEnqueuedElementPos();
	
	if (rcElementZone.Width() == 0) return CRect();

	return CRect(CPoint(rcElementZone.left, rcElementZone.top + m_nElementHeight - m_nElementAdrHeight),
		CSize(m_nElementWidth, m_nElementAdrHeight));

}

CRect CQueueDrawHelper::ComputeEnqueuedElementDataPos()
{
	CRect rcElementZone = ComputeEnqueuedElementPos();

	if (rcElementZone.Width() == 0) return CRect();

	return CRect(CPoint(rcElementZone.left + m_nMarge, rcElementZone.top + m_nElementAdrHeight + m_nMarge),
		CSize(m_nDataWidth, m_nDataHeight));
}

CRect CQueueDrawHelper::ComputeEnqueuedElementNextAdrPos()
{
	CRect rcElementZone = ComputeEnqueuedElementPos();

	if (rcElementZone.Width() == 0) return CRect();

	return CRect(CPoint(rcElementZone.left, rcElementZone.top + m_nElementHeight - m_nElementAdrHeight),
		CSize(m_nElementWidth, m_nElementAdrHeight));
}

CRect CQueueDrawHelper::ComputeHeadPointerPos()
{	
	CRect rcElement = ComputeHeadElementPos();
	return CRect(CPoint(rcElement.left, rcElement.top - 4 * m_nMarge), CSize(m_nElementWidth, m_nElementAdrHeight));
}

CPoint CQueueDrawHelper::ComputeHeadPointerOutArrowPos()
{
	CRect rcElement = ComputeHeadPointerPos();
	return CPoint(rcElement.left+ (rcElement.Width())/2, rcElement.bottom);
}

CRect CQueueDrawHelper::ComputeTailPointerPos()
{
	ASSERT(m_pQueueHelper);
	CRect rcRef;
	if (m_pQueueHelper->GetQueueItemsCount() == 0)
	{
		rcRef = ComputeHeadElementPos();
	}
	else
		rcRef = ComputeQueueElementPos(m_pQueueHelper->GetQueueItemsCount()-1);
	return CRect(CPoint(rcRef.left, rcRef.bottom + 2 * m_nMarge), CSize(m_nElementWidth, m_nElementAdrHeight));
}

CPoint CQueueDrawHelper::ComputeTailPointerOutArrowPos()
{
	CRect rcElement = ComputeTailPointerPos();
	return CPoint(rcElement.left + (rcElement.Width()) / 2, rcElement.top);
}

CRect CQueueDrawHelper::ComputePeekedDataPos()
{	
	CRect rcElement = ComputeQueueZone();	
	return CRect(CPoint(3 * m_nMarge, rcElement.bottom + m_nDataDistanceFromQueue), CSize(m_nDataWidth, m_nDataHeight));
}

CRect CQueueDrawHelper::ComputeEnqueuedDataPos()
{
	ASSERT(m_pQueueHelper);
	CRect rc;

	if (m_pQueueHelper->GetQueueItemsCount() == m_pQueueHelper->GetQueueMaxItemsCount())
		rc = ComputeQueueElementPos(m_pQueueHelper->GetQueueItemsCount()-1);
	else
		rc = ComputeEnqueuedElementZone();

	
	return CRect(CPoint(rc.left + (rc.Width() - m_nDataWidth) / 2, rc.bottom + m_nDataDistanceFromQueue), CSize(m_nDataWidth, m_nDataHeight));
	
}

