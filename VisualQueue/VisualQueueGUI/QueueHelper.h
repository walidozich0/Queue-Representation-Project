#pragma once
#include "../DataStruct/queue.h"

class CQueueHelper
{
public:
	CQueueHelper();
	~CQueueHelper();

	int GetQueueMaxItemsCount() { return m_nMaxItems; }

	CString GetTextRepresentation() { return m_strTextRepresentation; }
	CString GetTextRepresentationEx() { return m_strTextRepresentationEx; }
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
};


// class pour mesurer les positons des elements

class CQueueDrawHelper
{
public:
	CQueueDrawHelper();

	// set params
	void setQueueHelper(CQueueHelper* pQH) { m_pQueueHelper = pQH; }

	// 
	void Draw(CDC* pDC, CRect* pDrawCtrlRect);// CDC : Device Context


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

private:
	CQueueHelper* m_pQueueHelper;
	int m_nMarge;
	
	int m_nTopY;
	int m_nBottomY;

	int m_nDataWidth;
	int m_nDataHeight;
	int m_nDataDistanceFromQueue;

	int m_nElementWidth;
	int m_nElementHeight;
	int m_nElementAdrHeight;
	

	CSize m_DrawUnitLength;	

private:
	void _computeUnitsConvParams(CRect* pRealRC, CRect* pLogicalRC);
	RECT _convert(CRect* pRC);
	CRect _logicalUnits2DeviceUnits(CRect* pRC);
	CPoint _logicalUnits2DeviceUnits(CPoint* pPt);

};