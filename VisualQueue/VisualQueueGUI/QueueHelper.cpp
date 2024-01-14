#include "pch.h"
#include "QueueHelper.h"
#include "VisualQueueDrawerCtrl.h"




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


//////////////////////////////////////////

int _helperMinPointIndexInPtsSegment(CPoint& pt, CPoint& pt1, CPoint& pt2)//-1,0,1
{
	if (pt1 == pt2) return (pt == pt1) ? 1 : -1;
	if (pt == pt1) return 0;
	if (pt == pt2) return 1;
	if (pt1.x == pt2.x) return ((pt.x == pt1.x) && (pt.y >= min(pt1.y, pt2.y) && pt.y <= max(pt1.y, pt2.y))) ? 0 : -1;
	if (pt1.y == pt2.y) return ((pt.y == pt1.y) && (pt.x >= min(pt1.x, pt2.x) && pt.x <= max(pt1.x, pt2.x))) ? 0 : -1;

	// cas compliqué reporté ligne oblique

	ASSERT(FALSE);
	return -1;
}

int _helperMinPointIndexInPtsArray(CPoint* pPoints, int nCount, CPoint& point)
{
	ASSERT(nCount > 1);

	int ii = -1;

	for (int i = 0; i < (nCount - 1); i++)
	{
		ii = _helperMinPointIndexInPtsSegment(point, pPoints[i], pPoints[i + 1]);
		if (ii == 0) return i;
		if (ii == 1) return (i + 1);
	}
	return -1;
}

void _helperDrawConstrainedLine(CPoint* pPoints, int nCount, CPoint& point, Graphics* pGraphics, Pen* pPen, BOOL bDrawCurve)
{
	if (point == pPoints[0])
	{
		return;// on ne dessine rien
	}

	int nPointsToDrawCount = 0;
	Point* pPointsToDraw = NULL;

	if (point == pPoints[nCount - 1])
	{
		nPointsToDrawCount = nCount;
		pPointsToDraw = new Point[nPointsToDrawCount];
		_helperCopyPoints(pPoints, pPointsToDraw, nCount);
	}
	else
	{
		int iIndex = _helperMinPointIndexInPtsArray(pPoints, nCount, point);
		if (iIndex >= 0)
		{
			if (iIndex == (nCount - 1))// dernier point ?
			{
				ASSERT(FALSE);// en principe c traité précédemment
				nPointsToDrawCount = nCount;
				pPointsToDraw = new Point[nPointsToDrawCount];
				_helperCopyPoints(pPoints, pPointsToDraw, nCount);
			}
			else
			{
				nPointsToDrawCount = iIndex + 2;
				pPointsToDraw = new Point[nPointsToDrawCount];
				_helperCopyPoints(pPoints, pPointsToDraw, nPointsToDrawCount);
				if (point != pPoints[iIndex + 1])//changer le dernier elt
				{
					pPointsToDraw[iIndex + 1] = _helperConvertStruct(point);
				}
			}
		}
	}

	if (pPointsToDraw)
	{
		if (bDrawCurve)
			pGraphics->DrawCurve(pPen, pPointsToDraw, nPointsToDrawCount);
		else
			pGraphics->DrawLines(pPen, pPointsToDraw, nPointsToDrawCount);
	}

	if (pPointsToDraw) delete[] pPointsToDraw;

}

Point _helperConvertStruct(CPoint& pt)
{
	return Point(pt.x, pt.y);
}

Rect _helperConvertStruct(CRect& rc)
{
	return Rect(rc.left, rc.top, rc.Width(), rc.Height());
}

void _helperCopyPoints(CPoint* pSrcPoints, Point* pDestPoints, int nCount)
{
	if (pSrcPoints && pDestPoints && (nCount > 0))
	{
		for (int i = 0; i < nCount; i++) { pDestPoints[i] = _helperConvertStruct(pSrcPoints[i]); }
	}
}

void _helperCopyRects(CRect* pSrcRects, Rect* pDestRects, int nCount)
{
	if (pSrcRects && pDestRects && (nCount > 0))
	{
		for (int i = 0; i < nCount; i++) { pDestRects[i] = _helperConvertStruct(pSrcRects[i]); }
	}
}


void _helperDrawCircularPoint(CPoint pt, int nWidth,Graphics* pGraphics, Brush* pBrush)
{
	RectF r(pt.x - nWidth,pt.y - nWidth, 2 * nWidth, 2 * nWidth);
	pGraphics->FillEllipse(pBrush, r);
}

///////////////////////////////////////
// CQueueDrawSettings



CQueueDrawHelper::CQueueDrawHelper():
	m_pQueueHelper(NULL), m_pDrawingWindow(NULL)
	, m_fontFamily(L"Tahoma")
	, m_font(&m_fontFamily, 12, FontStyleRegular, UnitPoint)
	, m_solidBrush(Color(255, 0, 0, 0))
	, m_backgroundBrush(Color(255, 255, 255, 255))
	, m_backgroundBrushElt(Color(255, 153, 204, 255))
	, m_backgroundBrushAdr(Color(255, 204, 255, 204))
	, m_backgroundBrushAdrSuiv(Color(255, 255, 255, 204))
	, m_backgroundBrushData(Color(255, 204, 229, 255))
	, m_backgroundBrushEnqued(Color(150, 153, 153, 255))
	, m_backgroundBrushDequed(Color(255, 255, 178, 102))
	, m_backgroundBrushPeeked(Color(255, 255, 204, 229))
	, m_normalPen(Color(255, 0, 0, 0), 2.0F)
	, m_DataPen(Color(255, 0, 0, 0), 1.5F)
	, m_dashedPen(Color(255, 192, 192, 192), 2.0F)
	, m_dashedPen2(Color(255, 192, 192, 192), 1.5F)
	, m_arrowPen(Color(255, 128, 0, 0), 2.0F)
	, m_BlackArrowPen(Color(255, 0, 0, 0), 2.0F)

	, m_bAnimationEnabled(TRUE)
	, m_AnimMode(AnimMode::amNone)
	, m_AnimGeneralPen(Color(255, 255, 0, 0), 2.0F)
	, m_AnimDataAskingPen(Color(255, 255, 0, 0), 2.0F)
	, m_AnimArrowPen(Color(255, 255, 0, 0), 2.0F)
	, m_AnimMovingObjectBrush(Color(255, 255, 0, 0))
	
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
	m_rcDrawingWindowLogicalPosAndSize = m_rcDrawingWindowPhysicalPosAndSize = CRect();

	{
		// dashed pen
		REAL dashValues[2] = { 1, 1 };		
		m_dashedPen.SetDashPattern(dashValues, 2);
		REAL dashValues2[2] = { 1, 1 };		
		m_dashedPen2.SetDashPattern(dashValues2, 2);

		// arrow pen		
		AdjustableArrowCap myArrow(5, 5, true);
		myArrow.SetMiddleInset(2.0f);
		m_arrowPen.SetCustomEndCap(&myArrow);
		m_BlackArrowPen.SetCustomEndCap(&myArrow);

		// animpen
		m_AnimArrowPen.SetCustomEndCap(&myArrow);

	}
}

CQueueDrawHelper::~CQueueDrawHelper()
{
}

void CQueueDrawHelper::Draw()
{	
	ASSERT(m_pDrawingWindow && m_pDrawingWindow->GetSafeHwnd());

	CMemDC memDC((*m_pDrawingWindow->GetDC()), m_rcDrawingWindowPhysicalPosAndSize);
	Graphics graphics(memDC.GetDC());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	graphics.FillRectangle(&m_backgroundBrush, 0, 0, m_rcDrawingWindowPhysicalPosAndSize.Width(), m_rcDrawingWindowPhysicalPosAndSize.Height());

	// dequeue zone
	{

		CString strDequedValue;
		strDequedValue.Format(_T("%02d"), m_pQueueHelper->GetLastDequeuedValue());
		CRect rcLogical = ComputeDequeueDataPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillEllipse(&m_backgroundBrushDequed, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawEllipse(&m_DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(strDequedValue, -1, &m_font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);
	}

	{
		CRect rcLogical = ComputeDequeuedElementZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.DrawRectangle(&m_dashedPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
	}

	{
		CRect rcLogical = ComputeQueueZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		//graphics.DrawRectangle(&normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());		
		graphics.DrawLine(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.right, rcPhysical.top);
		graphics.DrawLine(&m_normalPen, rcPhysical.left, rcPhysical.bottom, rcPhysical.right, rcPhysical.bottom);
	}

	{
		CRect rcLogical = ComputeEnqueuedElementZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.DrawRectangle(&m_dashedPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());

	}

	{
		CRect rcLogical = ComputeQueueExtraZone();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);		
		graphics.DrawLine(&m_dashedPen2, rcPhysical.left, rcPhysical.top, rcPhysical.right, rcPhysical.top);
		graphics.DrawLine(&m_dashedPen2, rcPhysical.left, rcPhysical.bottom, rcPhysical.right, rcPhysical.bottom);
	}

	{
		CString strPeekedValue;
		if (m_AnimMode == AnimMode::amPeekOperation)
		{
			strPeekedValue = _T("??");
		}
		else
		{
			strPeekedValue.Format(_T("%02d"), m_pQueueHelper->GetLastPeekedValue());
		}
		
		CRect rcLogical = ComputePeekedDataPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillEllipse(&m_backgroundBrushPeeked, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawEllipse(&m_DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(strPeekedValue, -1, &m_font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);
	}


	{
		CString strEnquedValue;
		strEnquedValue.Format(_T("%02d"), m_pQueueHelper->GetLastEnqueuedValue());
		CRect rcLogical = ComputeEnqueuedDataPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillEllipse(&m_backgroundBrushData, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawEllipse(&m_DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(strEnquedValue, -1, &m_font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);
	}

	// queue zone
	for (int i = 0; i < m_pQueueHelper->GetQueueItemsCount(); i++)
	{
		CString strCurrentData = m_pQueueHelper->GetTextRepresentationEx().Mid(i * 10, 10);

		ASSERT(strCurrentData.GetLength() == 10);

		{
			CRect rcLogical = ComputeQueueElementPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			graphics.FillRectangle(&m_backgroundBrushElt, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawRectangle(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		}
		{
			CRect rcLogical = ComputeQueueElementAdrPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			graphics.FillRectangle(&m_backgroundBrushAdr, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawRectangle(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawString(strCurrentData.Left(4), -1, &m_font,
				RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);

		}
		{
			CRect rcLogical = ComputeQueueElementDataPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			graphics.FillEllipse(&m_backgroundBrushData, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawEllipse(&m_DataPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
			graphics.DrawString(strCurrentData.Mid(4, 2), -1, &m_font,
				RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);



		}
		{
			CRect rcLogical = ComputeQueueElementNextAdrPos(i);
			CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
			CString strCompareZero = _T("0000");

			if (strCurrentData.Right(4) == strCompareZero)
			{
				graphics.FillRectangle(&m_backgroundBrushAdrSuiv, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawRectangle(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawLine(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.right, rcPhysical.bottom);
			}
			else {
				graphics.FillRectangle(&m_backgroundBrushAdrSuiv, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawRectangle(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
				graphics.DrawString(strCurrentData.Right(4), -1, &m_font,
					RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);
			}


		}

		if (i != (m_pQueueHelper->GetQueueItemsCount() - 1))
		{


			CPoint ptStartLogical = ComputeQueueElementOutArrowPos(i);
			CPoint ptEndLogical = ComputeQueueElementInArrowPosFromPrevElt(i + 1);

			CPoint ptInterm1Logical = ptStartLogical;
			ptInterm1Logical.Offset((ptEndLogical.x - ptStartLogical.x) / 2, 0);
			CPoint ptInterm2Logical = ptEndLogical;
			ptInterm2Logical.Offset(-(ptEndLogical.x - ptStartLogical.x) / 2, 0);



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
			graphics.DrawCurve(&m_arrowPen, pointsColl, 4);
			//graphics.DrawBeziers(&arrowPen, pointsColl, 4);

		}

	}


	{
		CRect rcLogical = ComputeHeadPointerPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillRectangle(&m_backgroundBrushAdr, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawRectangle(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(_T("Head"), -1, &m_font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);
	}

	{
		if (m_pQueueHelper->GetQueueItemsCount() > 0)
		{
			CPoint ptStartLogical = ComputeHeadPointerOutArrowPos();
			CPoint ptEndLogical = ComputeQueueElementInArrowPosFromHeader(0);
			CPoint ptStartPhysical = _logicalUnits2DeviceUnits(&ptStartLogical);
			CPoint ptEndPhysical = _logicalUnits2DeviceUnits(&ptEndLogical);

			graphics.DrawLine(&m_BlackArrowPen, ptStartPhysical.x, ptStartPhysical.y, ptEndPhysical.x, ptEndPhysical.y);
		}
	}



	{
		CRect rcLogical = ComputeTailPointerPos();
		CRect rcPhysical = _logicalUnits2DeviceUnits(&rcLogical);
		graphics.FillRectangle(&m_backgroundBrushAdr, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawRectangle(&m_normalPen, rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height());
		graphics.DrawString(_T("Tail"), -1, &m_font,
			RectF(rcPhysical.left, rcPhysical.top, rcPhysical.Width(), rcPhysical.Height()), NULL, &m_solidBrush);
	}

	{
		if (m_pQueueHelper->GetQueueItemsCount() > 0)
		{
			CPoint ptStartLogical = ComputeTailPointerOutArrowPos();
			CPoint ptStartPhysical = _logicalUnits2DeviceUnits(&ptStartLogical);
			CPoint ptEndLogical = ComputeQueueElementInArrowPosFromTail(m_pQueueHelper->GetQueueItemsCount() - 1);
			CPoint ptEndPhysical = _logicalUnits2DeviceUnits(&ptEndLogical);
			graphics.DrawLine(&m_BlackArrowPen, ptStartPhysical.x, ptStartPhysical.y, ptEndPhysical.x, ptEndPhysical.y);
		}
	}


	// animation
	if (m_bAnimationEnabled)
	{
		_DrawAnimationForPeekOperation(&graphics);
	}
	
}

void CQueueDrawHelper::OnInit(CQueueHelper* pQH, CVisualQueueDrawerCtrl* pWnd)
{
	ASSERT(pQH);
	ASSERT(pWnd && pWnd->GetSafeHwnd());
	
	m_pQueueHelper = pQH;
	m_pDrawingWindow = pWnd;
	pWnd->m_pQueueDrawHelper = this;
	m_pDrawingWindow->ModifyStyle(0, SS_OWNERDRAW);

	m_rcDrawingWindowLogicalPosAndSize = computeCanvasRect();
	m_pDrawingWindow->GetClientRect(&m_rcDrawingWindowPhysicalPosAndSize);
	_computeUnitsConvParams();

	// anim init
	SetRelatedWnd(pWnd);
}

void CQueueDrawHelper::OnResizeWindow()
{
	ASSERT(m_pDrawingWindow && m_pDrawingWindow->GetSafeHwnd());
	m_pDrawingWindow->GetClientRect(&m_rcDrawingWindowPhysicalPosAndSize);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Anim Section



#define ANIM_PEEKOP_EXVAR_1_RC_HEADPOINTERPOS						101
#define ANIM_PEEKOP_EXVAR_2_PT_HEADPOINTEROUTARROWPOS				102
#define ANIM_PEEKOP_EXVAR_3_PT_QUEUEELEMENTINARROWPOSFROMHEADER		103
#define ANIM_PEEKOP_EXVAR_4_RC_ELEMENTDATAPOS						104
#define ANIM_PEEKOP_EXVAR_5_PT_ELEMENTDATAPOSOUTARROWPOS			105
#define ANIM_PEEKOP_EXVAR_6_RC_PEEKDATAPOS							106
#define ANIM_PEEKOP_EXVAR_7_PT_INTERM								107
#define ANIM_PEEKOP_EXVAR_8_PT_PEEKDATAINARROWPOS					108
#define ANIM_PEEKOP_EXVAR_9_PT_INTERM2								109
#define ANIM_PEEKOP_EXVAR_10_PT_HEADINARROWPOS						110



#define ANIM_PEEKOP_ID_0_PEEK_ASKING	100
#define ANIM_PEEKOP_ID_1_PEEK_TO_HEAD	101
#define ANIM_PEEKOP_ID_2_HEAD			102
#define ANIM_PEEKOP_ID_3_HEAD_DATA		103
#define ANIM_PEEKOP_ID_4_DATA_ARROW		104


#define ANIM_PEEKOP_SEQ_MAIN			101
#define ANIM_PEEKOP_SEQ_END				102

#define ANIM_PEEKOP_GRP_0_PEEK_ASKING	100
#define ANIM_PEEKOP_GRP_1_PEEK_TO_HEAD	101
#define ANIM_PEEKOP_GRP_2_HEAD			102
#define ANIM_PEEKOP_GRP_3_HEAD_DATA		103
#define ANIM_PEEKOP_GRP_4_DATA_ARROW	104

//#define ANIM_PEEKOP_GRP_4_DATA_TRANSF	104

#define ANIM_PEEKOP_ID_PEEK_WAITING		110
#define ANIM_PEEKOP_GRP_PEEK_WAITING	110


void CQueueDrawHelper::OnAllAnimationsDone()
{
	m_AnimMode = AnimMode::amNone;
}

void CQueueDrawHelper::_BuildAnimationForPeekOperation()
{
	ASSERT(m_pQueueHelper);
	
	Cleanup();
	

	// les variables nécessaires
	// calculs
	CRect rcHeadPointerPos = _logicalUnits2DeviceUnits(&ComputeHeadPointerPos());	
	CPoint ptHeadPointerOutArrowPos = _logicalUnits2DeviceUnits(&ComputeHeadPointerOutArrowPos());
	CPoint ptQueueElementInArrowPosFromHeader = _logicalUnits2DeviceUnits(&ComputeQueueElementInArrowPosFromHeader(0));
	CRect rcElementDataPos = _logicalUnits2DeviceUnits(&ComputeQueueElementDataPos(0));		
	CPoint ptElementDataPosOutArrowPos(rcElementDataPos.left, rcElementDataPos.top + rcElementDataPos.Height()/2);
	CRect rcPeekDataPos = _logicalUnits2DeviceUnits(&ComputePeekedDataPos());
	CPoint ptInterm(rcPeekDataPos.left + rcPeekDataPos.Width()/2, ptElementDataPosOutArrowPos.y);
	CPoint ptPeekDataInArrowPos(ptInterm.x, rcPeekDataPos.top);

	CPoint ptHeadInArrowPos(rcHeadPointerPos.left,rcHeadPointerPos.top + rcHeadPointerPos.Height()/2);
	CPoint ptInterm2(ptPeekDataInArrowPos.x,ptHeadInArrowPos.y);
	

	//defintion vars
	SetRectExtraVar(ANIM_PEEKOP_EXVAR_1_RC_HEADPOINTERPOS, rcHeadPointerPos);
	SetPointExtraVar(ANIM_PEEKOP_EXVAR_2_PT_HEADPOINTEROUTARROWPOS, ptHeadPointerOutArrowPos);
	SetPointExtraVar(ANIM_PEEKOP_EXVAR_3_PT_QUEUEELEMENTINARROWPOSFROMHEADER, ptQueueElementInArrowPosFromHeader);
	SetRectExtraVar(ANIM_PEEKOP_EXVAR_4_RC_ELEMENTDATAPOS, rcElementDataPos);
	SetPointExtraVar(ANIM_PEEKOP_EXVAR_5_PT_ELEMENTDATAPOSOUTARROWPOS, ptElementDataPosOutArrowPos);
	SetRectExtraVar(ANIM_PEEKOP_EXVAR_6_RC_PEEKDATAPOS, rcPeekDataPos);
	SetPointExtraVar(ANIM_PEEKOP_EXVAR_7_PT_INTERM, ptInterm);
	SetPointExtraVar(ANIM_PEEKOP_EXVAR_8_PT_PEEKDATAINARROWPOS, ptPeekDataInArrowPos);	
	SetPointExtraVar(ANIM_PEEKOP_EXVAR_9_PT_INTERM2, ptInterm2);
	SetPointExtraVar(ANIM_PEEKOP_EXVAR_10_PT_HEADINARROWPOS, ptHeadInArrowPos);
	
	// define animations

	//peek asking data ?
	{
		//VERIFY(DefineAnimationValue(ANIM_PEEKOP_ID_PEEK_WAITING, ANIM_PEEKOP_ID_PEEK_WAITING, 0, 5 * 360, 4 + 2 + 2 + 4 + 4));
	}

	// main anim sequence
	{
		//
		{
			VERIFY(DefineAnimationValue(ANIM_PEEKOP_ID_0_PEEK_ASKING, ANIM_PEEKOP_GRP_0_PEEK_ASKING, 0, 359, 2));
		}

		// peek to head
		// 
		{
			CPoint animPoints[3] = { ptPeekDataInArrowPos, ptInterm2, ptHeadInArrowPos };
			VERIFY(DefineAnimationPoint(ANIM_PEEKOP_ID_1_PEEK_TO_HEAD, ANIM_PEEKOP_GRP_1_PEEK_TO_HEAD, animPoints, 3, 1.0));
		}
		// header
		{
			CPoint animPoints[7] = { 
				CPoint(ptHeadPointerOutArrowPos.x - 1 ,ptHeadPointerOutArrowPos.y),//astuce pour la courbe !!!
				CPoint(rcHeadPointerPos.left,rcHeadPointerPos.bottom),
				rcHeadPointerPos.TopLeft(),
				CPoint(rcHeadPointerPos.right,rcHeadPointerPos.top),
				rcHeadPointerPos.BottomRight(),
				ptHeadPointerOutArrowPos,
				ptQueueElementInArrowPosFromHeader
			};
			VERIFY(DefineAnimationPoint(ANIM_PEEKOP_ID_2_HEAD, ANIM_PEEKOP_GRP_2_HEAD, animPoints, 7, 0.5));
		}

		// data : angle animation
		{
			VERIFY(DefineAnimationValue(ANIM_PEEKOP_ID_3_HEAD_DATA, ANIM_PEEKOP_GRP_3_HEAD_DATA, 0, 359,2));
		}

		//arrow
		{
			CPoint animPoints[3] = { ptElementDataPosOutArrowPos, ptInterm, ptPeekDataInArrowPos};
			VERIFY(DefineAnimationPoint(ANIM_PEEKOP_ID_4_DATA_ARROW, ANIM_PEEKOP_GRP_4_DATA_ARROW, animPoints, 3, 1.0));
			//VERIFY(DefineAnimationPoint(ANIM_PEEKOP_ID_4_DATA_TRANSF, ANIM_PEEKOP_GRP_4_DATA_TRANSF, animPoints, 3, 3));
		}

		// peek data : angle animation
		{
			//VERIFY(DefineAnimationValue(ANIM_PEEKOP_ID_5_PEEK_IN, ANIM_PEEKOP_GRP_5_PEEK_IN, 0, 359, 2));
		}

		//final		
		{
			// arrow hiding //???
			{

			}

			// delais (pas de visualisation
			{
				//VERIFY(DefineAnimationValue(ANIM_PEEKOP_ID_6_PEEK_END, ANIM_PEEKOP_GRP_6_PEEK_END, 0, 1, 1));
			}

		}

		DefineSequence(ANIM_PEEKOP_SEQ_MAIN, ANIM_PEEKOP_GRP_0_PEEK_ASKING, ANIM_PEEKOP_GRP_4_DATA_ARROW);
		
		// ajouter une methode qui stoppe un groupe à la fin d'une sequence
		// ajouter une methode qui stoppe un groupe à la fin d'un groupe
		// l'utiliser dans ce cas pour stopper ANIM_PEEKOP_ID_PEEK_WAITING
	}

	// finish anim ??? TBD
	{
		
		
	}

	

}

void CQueueDrawHelper::_DrawAnimationForPeekOperation(Graphics* pGraphics)
{
	if (m_AnimMode == AnimMode::amPeekOperation)
	{
		// peek data
		{
			CAnimationValue* pAnimValue = (CAnimationValue*)GetAnimationObjectById(ANIM_PEEKOP_ID_0_PEEK_ASKING);
			ASSERT(pAnimValue);
			CRect* pRectDataPeek = GetRectExtraVar(ANIM_PEEKOP_EXVAR_6_RC_PEEKDATAPOS);

			if (pAnimValue)
			{
				int nSweepAngle = (*pAnimValue);
				if (nSweepAngle >= 360) nSweepAngle = nSweepAngle % 360;

				if (nSweepAngle != 360)
				{
					pGraphics->DrawArc(&m_AnimDataAskingPen,
						pRectDataPeek->left, pRectDataPeek->top, pRectDataPeek->Width(), pRectDataPeek->Height(),
						-90, nSweepAngle);
				}
			}
		}
		// peek to head
		{
			CPoint* pPeekDataInArrowPosPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_8_PT_PEEKDATAINARROWPOS);
			CPoint* pIntermPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_9_PT_INTERM2);
			CPoint* pHeadInArrPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_10_PT_HEADINARROWPOS);
			ASSERT(pPeekDataInArrowPosPt && pIntermPt && pHeadInArrPt);

			CPoint refPoints[3] = { *pPeekDataInArrowPosPt, *pIntermPt, * pHeadInArrPt};
					

			CAnimationPoint* pAnimPoint = (CAnimationPoint*)GetAnimationObjectById(ANIM_PEEKOP_ID_1_PEEK_TO_HEAD);
			ASSERT(pAnimPoint);
			CPoint ptMoving = *pAnimPoint;
			if(ptMoving!=(*pHeadInArrPt)) _helperDrawConstrainedLine(refPoints, 3, ptMoving, pGraphics, &m_AnimArrowPen, FALSE);
		}

		// header pointer
		{
			CRect* pHeadPointerPosRect = GetRectExtraVar(ANIM_PEEKOP_EXVAR_1_RC_HEADPOINTERPOS);
			CPoint* pHeadPointerOutArrowPosPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_2_PT_HEADPOINTEROUTARROWPOS);
			CPoint* pQueueElementInArrowPosFromHeaderPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_3_PT_QUEUEELEMENTINARROWPOSFROMHEADER);

			ASSERT(pHeadPointerPosRect && pHeadPointerOutArrowPosPt && pQueueElementInArrowPosFromHeaderPt);			

			CPoint refPoints[7] = {
				CPoint(pHeadPointerOutArrowPosPt->x - 1 ,pHeadPointerOutArrowPosPt->y),//astuce pour la courbe !!!
				CPoint(pHeadPointerPosRect->left,pHeadPointerPosRect->bottom),
				pHeadPointerPosRect->TopLeft(),
				CPoint(pHeadPointerPosRect->right,pHeadPointerPosRect->top),
				pHeadPointerPosRect->BottomRight(),
				*pHeadPointerOutArrowPosPt,
				*pQueueElementInArrowPosFromHeaderPt
			};

			/*Point ptsToDraw1[7];
			_helperCopyPoints(refPoints, ptsToDraw1, 7);
			pGraphics->DrawLines(&m_dashedPen, ptsToDraw1, 7);*/

			CAnimationPoint* pAnimPoint = (CAnimationPoint*)GetAnimationObjectById(ANIM_PEEKOP_ID_2_HEAD);
			ASSERT(pAnimPoint);
			CPoint ptMoving = *pAnimPoint;
			_helperDrawConstrainedLine(refPoints, 7, ptMoving, pGraphics, &m_AnimGeneralPen, FALSE);
		}

		// data read
		{
			CAnimationValue* pAnimValue = (CAnimationValue*)GetAnimationObjectById(ANIM_PEEKOP_ID_3_HEAD_DATA);
			ASSERT(pAnimValue);
			CRect* pRectData = GetRectExtraVar(ANIM_PEEKOP_EXVAR_4_RC_ELEMENTDATAPOS);

			if (pAnimValue)
			{
				int nSweepAngle = (*pAnimValue);
				if (nSweepAngle > 360) nSweepAngle = nSweepAngle % 360;

				if(nSweepAngle != 360)
				{
					pGraphics->DrawArc(&m_AnimDataAskingPen,
						pRectData->left, pRectData->top, pRectData->Width(), pRectData->Height(),
						180, nSweepAngle);
				}
			}
		}

		// arrow transf 1st step
		{

			CPoint* pElementDataPosOutArrowPosPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_5_PT_ELEMENTDATAPOSOUTARROWPOS);
			CPoint* pIntermPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_7_PT_INTERM);
			CPoint* pPeekDataInArrowPosPt = GetPointExtraVar(ANIM_PEEKOP_EXVAR_8_PT_PEEKDATAINARROWPOS);
			ASSERT(pElementDataPosOutArrowPosPt && pIntermPt && pPeekDataInArrowPosPt);

			CPoint refPoints[3] = { *pElementDataPosOutArrowPosPt, *pIntermPt, *pPeekDataInArrowPosPt };
			
		/*	Point ptsToDraw1[3];
			_helperCopyPoints(refPoints, ptsToDraw1, 3);
			pGraphics->DrawLines(&m_dashedPen2, ptsToDraw1, 3);*/

			CAnimationPoint* pAnimPoint = (CAnimationPoint*)GetAnimationObjectById(ANIM_PEEKOP_ID_4_DATA_ARROW);
			ASSERT(pAnimPoint);
			CPoint ptMoving = *pAnimPoint;
			_helperDrawConstrainedLine(refPoints, 3, ptMoving, pGraphics, &m_AnimArrowPen, FALSE);
		}

		// 2nd step		
		//{
		//	CAnimationPoint* pAnimPoint = (CAnimationPoint*)GetAnimationObjectById(ANIM_PEEKOP_ID_4_DATA_TRANSF);
		//	ASSERT(pAnimPoint);			
		//	_helperDrawCircularPoint(*pAnimPoint,5, pGraphics, &m_AnimMovingObjectBrush);
		//}
	}
}


void CQueueDrawHelper::StartAnimationForPeekOperation()
{	
	if (m_pQueueHelper->GetLastPeekedValue() == -1) return;
	_BuildAnimationForPeekOperation();
	m_AnimMode = AnimMode::amPeekOperation;
	AnimateSequence(ANIM_PEEKOP_SEQ_MAIN);	
}


void CQueueDrawHelper::_computeUnitsConvParams()
{
	
	m_DrawUnitLength.cx = (m_rcDrawingWindowPhysicalPosAndSize.Size().cx) / (m_rcDrawingWindowLogicalPosAndSize.Width());
	m_DrawUnitLength.cy = (m_rcDrawingWindowPhysicalPosAndSize.Size().cy) / (m_rcDrawingWindowLogicalPosAndSize.Height());

	if (m_DrawUnitLength.cx < m_DrawUnitLength.cy) m_DrawUnitLength.cy = m_DrawUnitLength.cx;
	if (m_DrawUnitLength.cx > m_DrawUnitLength.cy) m_DrawUnitLength.cx = m_DrawUnitLength.cy;

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

