#pragma once
#include "../DataStruct/queue.h"

#include "AnimationControllerEx.h"

class CQueueHelper
{
public:
	CQueueHelper();
	~CQueueHelper();

	int GetQueueMaxItemsCount() { return m_nMaxItems; }

	CString GetTextRepresentation() { return m_strTextRepresentation; }
	CString GetTextRepresentationEx() { return m_strTextRepresentationEx; }
	CString GetLastDequeuedData() {	return m_strLastDequeuedData;}
	BOOL IsQueueInitialized();
	int GetQueueItemsCount();
	int GetLastEnqueuedValue() { return m_nLastEnqueuedValue; }
	int GetLastDequeuedValue() { return m_nLastDequeuedValue; }
	int GetLastPeekedValue() { return m_nLastPeekedValue; }	
	
	void InitQueue(int nItemsCount = -1);
	void Enqueue(int nValue);
	int Peek();
	int Dequeue();	

	void FreeQueue();


private: 
	void _initFileIfNot();
	void _generateTextData();

private:

	Queue* m_pQueue;
	
	CString m_strTextRepresentation;//h-> ... <-t
	CString m_strTextRepresentationEx;//xxxxddxxxxxxxxddxxxxxxxxddxxxxxxxxdd0000
	
	int m_nLastEnqueuedValue;
	int m_nLastDequeuedValue;
	int m_nLastPeekedValue;

	int m_nMaxItems;

	CString m_strLastDequeuedData;
};


class CVisualQueueDrawerCtrl;
class CVisualQueueGUIDlg;
// class pour mesurer les positons des elements

int _helperMinPointIndexInPtsSegment(CPoint& pt, CPoint& pt1, CPoint& pt2);//-1,0,1
int _helperMinPointIndexInPtsArray(CPoint* pPoints, int nCount, CPoint& point);
void _helperDrawConstrainedLine(CPoint* pPoints, int nCount, CPoint& point, Graphics* pGraphics, Pen* pPen, BOOL bDrawCurve = FALSE);


Point _helperConvertStruct(CPoint& pt);
Rect _helperConvertStruct(CRect& rc);
void _helperCopyPoints(CPoint* pSrcPoints, Point* pDestPoints, int nCount);
void _helperCopyRects(CRect* pSrcRects, Rect* pDestRects, int nCount);
void _helperDrawPoint(CPoint pt, int nWidth, Graphics* pGraphics, CBrush* pBrush);


class CQueueDrawHelper : public CAnimationControllerEx
{
public:
	CQueueDrawHelper();	
	virtual ~CQueueDrawHelper();
	// 
	void Draw();// CDC : Device Context

	void OnInit(CQueueHelper* pQH, CVisualQueueDrawerCtrl* pWnd, CVisualQueueGUIDlg* pDlg);
	void OnResizeWindow();

	enum AnimMode
	{
		amNone = 0,// pas d'animation
		amPeekOperation,
		amEnqueueOperation,
		amDequeueOperation
	};
	AnimMode  m_AnimMode;

	BOOL AnimationEnabled() { return m_bAnimationEnabled; }
	void EnableAnimation(BOOL bEnable) { m_bAnimationEnabled = bEnable; }
	void  StartAnimationForPeekOperation();
	void  StartAnimationForEnqueueOperation();
	void  StartAnimationForDequeueOperation();

private:
	// compute

	CRect computeCanvasRect();
	
	CRect ComputeDequeueDataPos();//1
	CRect ComputeDequeuedElementZone();//2	
	CRect ComputeDequeuedElementPos();//3
	CRect ComputeDequeuedElementAdrPos();//3
	CRect ComputeDequeuedElementDataPos();//4
	CRect ComputeDequeuedElementNextAdrPos();//5
	
	CRect ComputeQueueZone();//6,7,13,14	
	
	CRect ComputeHeadElementPos();//8
	CRect ComputeHeadElementAdrPos();//8
	CRect ComputeHeadElementDataPos();//9
	CRect ComputeHeadElementNextAdrPos();//10
	
	CRect ComputeQueueElementPos(int index);//
	CRect ComputeQueueElementAdrPos(int index);//
	CRect ComputeQueueElementDataPos(int index);//
	CRect ComputeQueueElementNextAdrPos(int index);//
	CPoint ComputeQueueElementInArrowPosFromHeader(int index);
	CPoint ComputeQueueElementInArrowPosFromTail(int index);
	CPoint ComputeQueueElementInArrowPosFromPrevElt(int index);
	CPoint ComputeQueueElementOutArrowPos(int index);
	
	CRect ComputeEnqueuedElementZone();//2	
	CRect ComputeEnqueuedElementPos();//3
	CRect ComputeEnqueuedElementAdrPos();//3
	CRect ComputeEnqueuedElementDataPos();//4
	CRect ComputeEnqueuedElementNextAdrPos();//5
	
	CRect ComputeHeadPointerPos();//11
	CPoint ComputeHeadPointerOutArrowPos();
	
	CRect ComputeTailPointerPos();//16
	CPoint ComputeTailPointerOutArrowPos();
	
	CRect ComputePeekedDataPos();//12
	CRect ComputeEnqueuedDataPos();//15


	CRect ComputeQueueExtraZone();//17,18

public:
	virtual void OnAllAnimationsDone();
private:	
	void  _BuildAnimationForPeekOperation();
	void  _DrawAnimationForPeekOperation(Graphics* pGraphics);

	void  _BuildAnimationForEnqueueOperation();
	void  _DrawAnimationForEnqueueOperation(Graphics* pGraphics);

	void  _BuildAnimationForDequeueOperation();
	void  _DrawAnimationForDequeueOperation(Graphics* pGraphics);
	
	
	

private:
	CQueueHelper* m_pQueueHelper;	
	CWnd* m_pDrawingWindow;
	CVisualQueueGUIDlg* m_pMainWnd;
	
	CRect m_rcDrawingWindowLogicalPosAndSize;
	CRect m_rcDrawingWindowPhysicalPosAndSize;
	CSize m_DrawUnitLength;
	
	int m_nMarge;	
	int m_nTopY;
	int m_nBottomY;
	int m_nDataWidth;
	int m_nDataHeight;
	int m_nDataDistanceFromQueue;
	int m_nElementWidth;
	int m_nElementHeight;
	int m_nElementAdrHeight;


	FontFamily   m_fontFamily;
	Gdiplus::Font m_font;
	Pen m_normalPen;
	Pen m_DataPen;
	Pen m_dashedPen;
	Pen m_dashedPen2;
	Pen m_arrowPen;
	Pen m_BlackArrowPen;
	SolidBrush  m_solidBrush;
	SolidBrush m_backgroundBrush;
	SolidBrush m_backgroundBrushElt;
	SolidBrush m_backgroundBrushAdr;
	SolidBrush m_backgroundBrushAdrSuiv;
	SolidBrush m_backgroundBrushData;
	SolidBrush m_backgroundBrushEnqued;
	SolidBrush m_backgroundBrushDequed;
	SolidBrush m_backgroundBrushPeeked;

	BOOL m_bAnimationEnabled;
	
	Pen m_AnimGeneralPen;
	Pen m_AnimDataAskingPen;
	Pen m_AnimArrowPen;
	SolidBrush m_AnimMovingObjectBrush;

private:
	void _computeUnitsConvParams();
	
	CRect _logicalUnits2DeviceUnits(CRect* pRC);
	CPoint _logicalUnits2DeviceUnits(CPoint* pPt);
	int	_logicalUnits2DeviceUnitsX(int nInput);

};