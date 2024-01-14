#include "pch.h"
#include "QueueHelper.h"

CQueueHelper::CQueueHelper():
	m_pFile(NULL)
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
		else if (GetQueueItemsCount()==0) m_strTextRepresentation = _T("(File Vide)");
		else
		{
			m_strTextRepresentation = _T("H");
			
			Element* pElement = m_pFile->premier;
			while (pElement)
			{
				CString strTemp;
				strTemp.Format(_T("->(%2d)"),pElement->nombre);
				m_strTextRepresentation += strTemp;
				pElement = pElement->suivant;
			}			

			m_strTextRepresentation += _T("<-T");
			//H -> (00) <- T
			//H -> (00) -> (00) -> (00) <- T
			
		}
	}
	return m_strTextRepresentation;
}

BOOL CQueueHelper::IsQueueInitialized()
{
	return m_pFile!=NULL;
}

void CQueueHelper::InitQueue(int nItemsCount)
{
	FreeQueue();
	_initFileIfNot();
	if (nItemsCount > 0)
	{		
		
		::file_ajouter_aleatoires(m_pFile, nItemsCount);
		GetTextRepresentation(TRUE);
	}

}

void CQueueHelper::Enqueue(int nValue)
{
	_initFileIfNot();
	::enfiler(m_pFile, nValue);
	m_nLastEnqueuedValue = nValue;
	GetTextRepresentation(TRUE);// regenerer la represenattion textuelle
}

int CQueueHelper::Peek()
{
	if (!IsQueueInitialized()) return -1;
	m_nLastPeekedValue = ::peek(m_pFile);
}

int CQueueHelper::Dequeue()
{
	if (!IsQueueInitialized()) return -1;
	m_nLastDequeuedValue = ::defiler(m_pFile);
	GetTextRepresentation(TRUE);// regenerer la represenattion textuelle
}

int CQueueHelper::GetQueueItemsCount()
{
	if (!IsQueueInitialized()) return -1;
	return m_pFile->nb_elements;
}

void CQueueHelper::FreeQueue()
{
	if (m_pFile)
	{
		::file_vider(m_pFile);
		::free(m_pFile);
		m_pFile = NULL;
	}
	m_strTextRepresentation = _T("(File Non Initialisée)");
	m_nLastEnqueuedValue = m_nLastDequeuedValue = m_nLastPeekedValue = -1;
}

void CQueueHelper::_initFileIfNot()
{
	if (!m_pFile)
	{
		m_pFile = initialiser();
		m_strTextRepresentation = _T("(File Vide)");
		m_nLastEnqueuedValue = m_nLastDequeuedValue = m_nLastPeekedValue = -1;
	}
}
