// CMyAnimationController.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "AnimationControllerEx.h"


// CMyAnimationController


CAnimationControllerEx::CAnimationControllerEx() :
	m_bIsPaused(FALSE)
{
	VERIFY(EnableAnimationTimerEventHandler());
	VERIFY(EnableAnimationManagerEvent());
}

CAnimationControllerEx::~CAnimationControllerEx()
{
	Cleanup();
}

void CAnimationControllerEx::OnStoryboardStatusChanged(CAnimationGroup* pGroup, UI_ANIMATION_STORYBOARD_STATUS newStatus, UI_ANIMATION_STORYBOARD_STATUS previousStatus)
{
	CAnimationController::OnStoryboardStatusChanged(pGroup, newStatus, previousStatus);

	if (previousStatus == UI_ANIMATION_STORYBOARD_FINISHED)
	{
		UINT uGroupId = pGroup->GetGroupID();
		OnGroupAnimationCompleted(uGroupId);
		// triggers
		CList<DWORD, DWORD&>* pListTriggers = GetTriggerList(uGroupId);
		if (pListTriggers && pListTriggers->GetCount() > 0)
		{
			POSITION pos = pListTriggers->GetHeadPosition();
			while (pos)
			{
				DWORD dwTriggeredGrp = pListTriggers->GetNext(pos);
				if (HIWORD(dwTriggeredGrp) == 1)
				{
					ScheduleGroup(LOWORD(dwTriggeredGrp));
				}
				else
				{
					CleanUpGroup(LOWORD(dwTriggeredGrp));
				}
			}
		}
	}

	if (previousStatus == UI_ANIMATION_STORYBOARD_FINISHED && newStatus != UI_ANIMATION_STORYBOARD_FINISHED)
	{
		UINT uGroupId = pGroup->GetGroupID();
		WORD wSeqID = 0;
		if (m_mapGrpToSeq.Lookup(uGroupId, wSeqID) && (wSeqID > 0))
		{
			DWORD wSeqRange = 0;
			VERIFY(m_SequenceDefs.Lookup(wSeqID, wSeqRange));
			UINT uLastGroupId = HIWORD(wSeqRange);
			ASSERT(uLastGroupId > 0);
			ASSERT(uGroupId <= uLastGroupId);

			if (uGroupId == uLastGroupId)// sequence terminée
			{
				int nPlaysCount = 0;//sequence en boucle
				if (m_ExecutedAnimationLoops_.Lookup(wSeqID, nPlaysCount))//sequence en boucle
				{
					nPlaysCount--;
					if (nPlaysCount > 0)// replanifier
					{
						UINT uFirstGroupId = LOWORD(wSeqRange);
						ASSERT(uFirstGroupId > 0);
						m_ExecutedAnimationLoops_.SetAt(wSeqID, nPlaysCount);
						CleanupSequenceGroups(wSeqID);
						VERIFY(ScheduleGroup(uFirstGroupId));
					}
					else
					{
						m_ExecutedAnimationLoops_.RemoveKey(wSeqID);
						OnSeqAnimationCompleted(wSeqID);
					}
				}
				else
				{
					OnSeqAnimationCompleted(wSeqID);
				}
			}
			else
			{
				ScheduleGroup(uGroupId + 1);
			}
		}
	}
}

void CAnimationControllerEx::OnAnimationManagerStatusChanged(UI_ANIMATION_MANAGER_STATUS newStatus, UI_ANIMATION_MANAGER_STATUS previousStatus)
{
	CAnimationController::OnAnimationManagerStatusChanged(newStatus, previousStatus);
	if (newStatus == UI_ANIMATION_MANAGER_IDLE && previousStatus == UI_ANIMATION_MANAGER_BUSY && !m_bIsPaused)
	{
		OnAllAnimationsDone();
	}
}

void CAnimationControllerEx::OnBeforeAnimationStart(CAnimationGroup* pGroup)
{
	CAnimationController::OnBeforeAnimationStart(pGroup);
	OnGroupAnimationStarting(pGroup->GetGroupID());
}


// CMyAnimationController member functions

BOOL CAnimationControllerEx::StopAllAnimations()
{
	IUIAnimationManager* pManager = GetUIAnimationManager();
	if (!pManager) { ASSERT(FALSE); return FALSE; }
	m_bIsPaused = FALSE;
	HRESULT hr = pManager->AbandonAllStoryboards();		
	if (SUCCEEDED(hr)) return TRUE;
	else { ASSERT(FALSE); return FALSE; }
}

BOOL CAnimationControllerEx::PauseAllAnimations()
{
	IUIAnimationManager* pManager = GetUIAnimationManager();
	if (!pManager) { ASSERT(FALSE); return FALSE; }
	m_bIsPaused = TRUE;
	HRESULT hr = pManager->Pause();
	if (SUCCEEDED(hr)) return TRUE;
	else { ASSERT(FALSE); return FALSE; }
}

BOOL CAnimationControllerEx::ResumeAllAnimations()
{
	IUIAnimationManager* pManager = GetUIAnimationManager();
	if (!pManager) { ASSERT(FALSE); return FALSE; }
	m_bIsPaused = FALSE;
	HRESULT hr = pManager->Resume();
	if (SUCCEEDED(hr)) return TRUE;
	else { ASSERT(FALSE); return FALSE; }
}

BOOL CAnimationControllerEx::StopGroupAnimations(UINT uGroupId)
{
	ASSERT(FALSE);
	return FALSE;
	// not working ????	
	POSITION pos = m_lstAnimationGroups.GetHeadPosition();
	while (pos)
	{
		CAnimationGroup* pGrp = m_lstAnimationGroups.GetNext(pos);
		if (pGrp->GetGroupID() == uGroupId)	
		{			
			pGrp->RemoveKeyframes();
			return TRUE;
		}
	}
	return FALSE;

}

void CAnimationControllerEx::OnSeqAnimationCompleted(WORD wSeqId)
{
	TRACE1("*** Sequence AnimationCompleted (%d)", wSeqId);

}

void CAnimationControllerEx::OnGroupAnimationStarting(DWORD dwGrpId)
{
	TRACE1("Group Animation Starting (%d)", dwGrpId);
}

void CAnimationControllerEx::OnGroupAnimationCompleted(DWORD dwGrpId)
{
	TRACE1("Group Animation Completed (%d)", dwGrpId);
}

void CAnimationControllerEx::OnAllAnimationsDone()
{
	TRACE("All Animations Completed ");
}

CAnimationPoint* CAnimationControllerEx::DefineAnimationPoint(WORD wAnimObjId, UINT nGroupId)
{
	LPVOID lpDummy = NULL;
	if (m_MapIdObjToAnimObj.Lookup(wAnimObjId, lpDummy)) { ASSERT(FALSE); return NULL; }

	CAnimationPoint* pAnimationPoint = new CAnimationPoint();
	pAnimationPoint->SetID(wAnimObjId, nGroupId);

	CAnimationGroup* pGrp = AddAnimationObject(pAnimationPoint);
	pGrp->m_bAutodestroyAnimationObjects = TRUE;


	m_MapIdObjToAnimObj.SetAt(wAnimObjId, pAnimationPoint);

	return pAnimationPoint;
}

CAnimationPoint* CAnimationControllerEx::DefineAnimationPoint(WORD wAnimObjId, UINT nGroupId, CPoint ptStart, CPoint ptEnd, UI_ANIMATION_SECONDS nDuration)
{
	CAnimationPoint* pAnimationPoint = DefineAnimationPoint(wAnimObjId, nGroupId);
	ASSERT(pAnimationPoint);

	if (pAnimationPoint)
	{
		*pAnimationPoint = ptStart;

		pAnimationPoint->AddTransition(
			new CAccelerateDecelerateTransition(nDuration, ptEnd.x),
			new CAccelerateDecelerateTransition(nDuration, ptEnd.y));
	}

	return pAnimationPoint;
}

CAnimationPoint* CAnimationControllerEx::DefineAnimationPoint(WORD wAnimObjId, UINT nGroupId, CPoint* pPoints, int nCount, UI_ANIMATION_SECONDS nDuration)
{
	CAnimationPoint* pAnimationPoint = DefineAnimationPoint(wAnimObjId, nGroupId);
	ASSERT(pAnimationPoint);

	if (pAnimationPoint)
	{
		if (pPoints && (nCount > 0))
		{
			*pAnimationPoint = pPoints[0];
			for (int i = 1; i < nCount; i++)
				pAnimationPoint->AddTransition( 
					new CAccelerateDecelerateTransition(nDuration, pPoints[i].x),
					new CAccelerateDecelerateTransition(nDuration, pPoints[i].y));
		}
	}

	return pAnimationPoint;
}

CAnimationRect* CAnimationControllerEx::DefineAnimationRect(WORD wAnimObjId, UINT nGroupId)
{
	LPVOID lpDummy = NULL;
	if (m_MapIdObjToAnimObj.Lookup(wAnimObjId, lpDummy)) { ASSERT(FALSE); return NULL; }

	CAnimationRect* pAnimationRect = new CAnimationRect();
	pAnimationRect->SetID(wAnimObjId, nGroupId);
	CAnimationGroup* pGrp = AddAnimationObject(pAnimationRect);
	pGrp->m_bAutodestroyAnimationObjects = TRUE;

	m_MapIdObjToAnimObj.SetAt(wAnimObjId, pAnimationRect);
	return pAnimationRect;
}

CAnimationRect* CAnimationControllerEx::DefineAnimationRect(WORD wAnimObjId, UINT nGroupId, CRect rcStart, CRect rcFinish, UI_ANIMATION_SECONDS nDuration)
{
	CAnimationRect* pAnimationRect = DefineAnimationRect(wAnimObjId, nGroupId);
	ASSERT(pAnimationRect);

	if (pAnimationRect)
	{
		*pAnimationRect = rcStart;

		pAnimationRect->AddTransition(
			new CAccelerateDecelerateTransition(nDuration, rcFinish.left),
			new CAccelerateDecelerateTransition(nDuration, rcFinish.top),
			new CAccelerateDecelerateTransition(nDuration, rcFinish.right),
			new CAccelerateDecelerateTransition(nDuration, rcFinish.bottom));
	}

	return pAnimationRect;
}

CAnimationRect* CAnimationControllerEx::DefineAnimationRect(WORD wAnimObjId, UINT nGroupId, CRect* pRects, int nCount, UI_ANIMATION_SECONDS nDuration)
{
	CAnimationRect* pAnimationRect = DefineAnimationRect(wAnimObjId, nGroupId);
	ASSERT(pAnimationRect);

	if (pAnimationRect)
	{
		if (pRects && (nCount > 0))
		{
			*pAnimationRect = pRects[0];
			for (int i = 1; i < nCount; i++)
				pAnimationRect->AddTransition(
					new CAccelerateDecelerateTransition(nDuration, pRects[i].left),
					new CAccelerateDecelerateTransition(nDuration, pRects[i].top),
					new CAccelerateDecelerateTransition(nDuration, pRects[i].right),
					new CAccelerateDecelerateTransition(nDuration, pRects[i].bottom));
		}
	}

	return pAnimationRect;
}

CAnimationValue* CAnimationControllerEx::DefineAnimationValue(WORD wAnimObjId, UINT nGroupId)
{

	LPVOID lpDummy = NULL;
	if (m_MapIdObjToAnimObj.Lookup(wAnimObjId, lpDummy)) { ASSERT(FALSE); return NULL; }


	CAnimationValue* pAnimationValue = new CAnimationValue();
	pAnimationValue->SetID(wAnimObjId, nGroupId);
	CAnimationGroup* pGrp = AddAnimationObject(pAnimationValue);
	pGrp->m_bAutodestroyAnimationObjects = TRUE;


	m_MapIdObjToAnimObj.SetAt(wAnimObjId, pAnimationValue);

	return pAnimationValue;
}

CAnimationValue* CAnimationControllerEx::DefineAnimationValue(WORD wAnimObjId, UINT nGroupId, int nStart, int nEnd, UI_ANIMATION_SECONDS nDuration)
{
	CAnimationValue* pAnimationValue = DefineAnimationValue(wAnimObjId, nGroupId);
	ASSERT(pAnimationValue);

	if (pAnimationValue)
	{
		(*pAnimationValue) = nStart;
		pAnimationValue->AddTransition(new CAccelerateDecelerateTransition(nDuration, nEnd));
	}

	return pAnimationValue;
}

CAnimationBaseObject* CAnimationControllerEx::GetAnimationObjectById(WORD wObjectId)
{
	LPVOID lpData = NULL;
	if (!m_MapIdObjToAnimObj.Lookup(wObjectId, lpData)) { return NULL; }
	return (CAnimationBaseObject*)lpData;
}

BOOL CAnimationControllerEx::DefineSequence(WORD wSeqId, UINT nStartGroupId, UINT nEndGroupId)
{
	if ((wSeqId <= 0) || (nStartGroupId <= 0) || (nEndGroupId <= 0) || (nStartGroupId > nEndGroupId))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	DWORD dwSeqRange = 0;
	if (m_SequenceDefs.Lookup(wSeqId, dwSeqRange))
	{
		if (nStartGroupId != ((HIWORD(dwSeqRange)) + 1))
		{
			ASSERT(FALSE);
			return FALSE;
		}

		dwSeqRange = MAKELONG(LOWORD(dwSeqRange), nEndGroupId);
		m_SequenceDefs.SetAt(wSeqId, dwSeqRange);
	}
	else
	{
		dwSeqRange = MAKELONG(nStartGroupId, nEndGroupId);
		m_SequenceDefs.SetAt(wSeqId, dwSeqRange);
	}

	for (UINT i = nStartGroupId; i <= nEndGroupId; i++)
	{
		WORD w = 0;
		if (m_mapGrpToSeq.Lookup(i, w)) { ASSERT(FALSE); }
		m_mapGrpToSeq.SetAt(i, wSeqId);
	}

	return TRUE;
}

BOOL CAnimationControllerEx::AnimateSequence(WORD wSeqId, int nRepeat)
{
	if (wSeqId <= 0) { ASSERT(FALSE); return FALSE; }

	DWORD dwRange = 0;
	if (!m_SequenceDefs.Lookup(wSeqId, dwRange))
	{
		ASSERT(FALSE); return FALSE;
	}

	if (nRepeat > 1)
	{
		int n = 0;
		if (m_ExecutedAnimationLoops_.Lookup(wSeqId, n)) { ASSERT(FALSE); }

		m_ExecutedAnimationLoops_.SetAt(wSeqId, nRepeat);
	}

	UINT nStartGroupId = LOWORD(dwRange);
	UINT nEndGroupId = HIWORD(dwRange);

	for (UINT ii = nStartGroupId; ii <= nEndGroupId; ii++)
	{
		VERIFY(AnimateGroup(ii, FALSE));
		VERIFY(EnableStoryboardEventHandler(ii, TRUE));
	}

	VERIFY(ScheduleGroup(nStartGroupId));
}

void CAnimationControllerEx::CleanupSequenceGroups(WORD wSeqId)
{
	if (wSeqId <= 0) { ASSERT(FALSE); return; }

	DWORD dwRange = 0;
	if (!m_SequenceDefs.Lookup(wSeqId, dwRange))
	{
		ASSERT(FALSE); return;
	}


	UINT nStartGroupId = LOWORD(dwRange);
	UINT nEndGroupId = HIWORD(dwRange);


	for (UINT ii = nStartGroupId; ii <= nEndGroupId; ii++) CleanUpGroup(ii);

}

void CAnimationControllerEx::SetPointExtraVar(WORD wId, CPoint pt)
{
	LPVOID lpData = NULL;
	if (m_ptExtraVars.Lookup(wId, lpData))
	{
		CPoint* pPoint = (CPoint*)lpData;
		(*pPoint) = pt;
	}
	else
	{
		CPoint* pPoint = new CPoint(pt);
		m_ptExtraVars.SetAt(wId, pPoint);
	}
}

CPoint* CAnimationControllerEx::GetPointExtraVar(WORD wId)
{
	LPVOID lpData = NULL;
	if (m_ptExtraVars.Lookup(wId, lpData))
		return (CPoint*)lpData;
	else
		return NULL;
}

void CAnimationControllerEx::SetRectExtraVar(WORD wId, CRect rc)
{
	LPVOID lpData = NULL;
	if (m_rcExtraVars.Lookup(wId, lpData))
	{
		CRect* pRect = (CRect*)lpData;
		(*pRect) = rc;
	}
	else
	{
		CRect* pRect = new CRect(rc);
		m_ptExtraVars.SetAt(wId, pRect);
	}

}

CRect* CAnimationControllerEx::GetRectExtraVar(WORD wId)
{
	LPVOID lpData = NULL;
	if (m_ptExtraVars.Lookup(wId, lpData))
		return (CRect*)lpData;
	else
		return NULL;
}

void CAnimationControllerEx::TriggerAnimation(WORD wGrpId, WORD wTriggeredAnimGrp, BOOL bOn)
{
	CList<DWORD, DWORD&>* pListTriggers = NULL;
	LPVOID lpData = NULL;
	if (m_mapToTriggers.Lookup(wGrpId, lpData))
	{
		pListTriggers = (CList<DWORD, DWORD&>*)lpData;
	}
	else
	{
		pListTriggers = new CList<DWORD, DWORD&>();
		m_mapToTriggers.SetAt(wGrpId, pListTriggers);
	}
	
	ASSERT(pListTriggers);
	DWORD dwTriggeredGrp = MAKELONG(wTriggeredAnimGrp, bOn ? 1:0);

#ifdef _DEBUG
	{
		POSITION pos = pListTriggers->GetHeadPosition();
		while (pos)
		{
			POSITION posOld = pos;
			DWORD dwItem = pListTriggers->GetAt(pos);

			if (LOWORD(dwItem) == wTriggeredAnimGrp)
			{
				ASSERT(FALSE);
				return;
			}
			pListTriggers->GetNext(pos);
		}
	}
#endif
	
	pListTriggers->AddTail(dwTriggeredGrp);

	if(bOn) VERIFY(AnimateGroup(wTriggeredAnimGrp, FALSE));
}

CList<DWORD, DWORD&>* CAnimationControllerEx::GetTriggerList(WORD wGrpId)
{
	LPVOID lpData = NULL;
	if (m_mapToTriggers.Lookup(wGrpId, lpData))
	{
		return (CList<DWORD, DWORD&>*)lpData;
	}
	else
	{
		return NULL;
	}
}


void CAnimationControllerEx::Cleanup()
{
	m_MapIdObjToAnimObj.RemoveAll();


	m_mapGrpToSeq.RemoveAll();
	m_SequenceDefs.RemoveAll();
	m_ExecutedAnimationLoops_.RemoveAll();

	{
		POSITION pos = m_ptExtraVars.GetStartPosition();
		while (pos)
		{
			WORD wKey = 0;
			CPoint* pPoint = NULL;
			m_ptExtraVars.GetNextAssoc(pos, wKey, (LPVOID&)pPoint);
			delete pPoint;
		}
		m_ptExtraVars.RemoveAll();
	}

	{
		POSITION pos = m_rcExtraVars.GetStartPosition();
		while (pos)
		{
			WORD wKey = 0;
			CRect* pRect = NULL;
			m_rcExtraVars.GetNextAssoc(pos, wKey, (LPVOID&)pRect);
			if (pRect) delete pRect;
		}
		m_rcExtraVars.RemoveAll();
	}

	{
		POSITION pos = m_mapToTriggers.GetStartPosition();
		while (pos)
		{
			WORD wKey = 0;
			CList<DWORD,DWORD&>* pList = NULL;
			m_mapToTriggers.GetNextAssoc(pos, wKey, (LPVOID&)pList);
			if(pList) delete pList;
		}
		m_mapToTriggers.RemoveAll();
	}

}
