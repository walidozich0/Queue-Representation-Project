#pragma once
#include "../DataStruct/queue.h"

class CQueueHelper
{
public:
	CQueueHelper();
	~CQueueHelper();

	CString GetTextRepresentation(BOOL bRegenerate = FALSE);
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

private:

	File* m_pFile;
	
	CString m_strTextRepresentation;
	
	int m_nLastEnqueuedValue;
	int m_nLastDequeuedValue;
	int m_nLastPeekedValue;
};


