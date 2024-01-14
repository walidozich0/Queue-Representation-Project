#include "pch.h"
#include "QueueHelper.h"

CQueueHelper::CQueueHelper():
	m_pQueue(NULL)
	, m_strTextRepresentation(_T("(File Non Initialisée)"))
	, m_nLastEnqueuedValue(-1)
	, m_nLastDequeuedValue(-1)
	, m_nLastPeekedValue(-1)
{
	TRACE("\r\n******* Constructeur CQueueHelper appelé\r\n");
	
}

CQueueHelper::~CQueueHelper()
{
	TRACE("\r\n******* Destructeur CQueueHelper appelé\r\n");
	FreeQueue();
}

CString CQueueHelper::GetTextRepresentation(BOOL bRegenerate)
{
	if (bRegenerate)
	{
		if (!IsQueueInitialized()) m_strTextRepresentation = _T("(File Non Initialisée)");
		else if (GetQueueItemsCount()==0) m_strTextRepresentation = _T("(File existe mais Vide)");
		else
		{
			m_strTextRepresentation = _T("Head");
			
			QueueEntry* pEntry = m_pQueue->m_pHeadEntry;
			while (pEntry)
			{
				CString strTemp;
				strTemp.Format(_T("->(%02d)"),pEntry->m_nData);
				m_strTextRepresentation += strTemp;
				pEntry = pEntry->m_pNextEntry;
			}			

			m_strTextRepresentation += _T("<-Tail");
			
		}
	}
	return m_strTextRepresentation;
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
		GetTextRepresentation(TRUE);
	}

}

void CQueueHelper::Enqueue(int nValue)
{
	_initFileIfNot();
	::Enqueue(m_pQueue, nValue);
	m_nLastEnqueuedValue = nValue;
	GetTextRepresentation(TRUE);// regenerer la represenattion textuelle
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
	GetTextRepresentation(TRUE);// regenerer la represenattion textuelle
}

int CQueueHelper::GetQueueItemsCount()
{
	if (!IsQueueInitialized()) return -1;
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
