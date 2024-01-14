#pragma once


// CMyAnimationController command target

class CAnimationControllerEx : public CAnimationController
{
public:
	CAnimationControllerEx();
	virtual ~CAnimationControllerEx();


	virtual void OnStoryboardStatusChanged(
		CAnimationGroup* pGroup,
		UI_ANIMATION_STORYBOARD_STATUS newStatus,
		UI_ANIMATION_STORYBOARD_STATUS previousStatus);

	virtual void OnAnimationManagerStatusChanged(
		UI_ANIMATION_MANAGER_STATUS newStatus,
		UI_ANIMATION_MANAGER_STATUS previousStatus);

	virtual void OnBeforeAnimationStart(CAnimationGroup* pGroup);



	BOOL StopAllAnimations();
	BOOL PauseAllAnimations();
	BOOL ResumeAllAnimations();
	BOOL StopGroupAnimations(UINT uGroupId);

	virtual void OnSeqAnimationCompleted(WORD wSeqId);
	virtual void OnGroupAnimationStarting(DWORD dwGrpId);
	virtual void OnGroupAnimationCompleted(DWORD dwGrpId);
	virtual void OnAllAnimationsDone();

	CAnimationPoint* DefineAnimationPoint(WORD wAnimObjId, UINT nGroupId);
	CAnimationPoint* DefineAnimationPoint(WORD wAnimObjId, UINT nGroupId, CPoint ptStart, CPoint ptEnd, UI_ANIMATION_SECONDS nDuration);
	CAnimationPoint* DefineAnimationPoint(WORD wAnimObjId, UINT nGroupId, CPoint* pPoints, int nCount, UI_ANIMATION_SECONDS nDuration);

	CAnimationRect* DefineAnimationRect(WORD wAnimObjId, UINT nGroupId);
	CAnimationRect* DefineAnimationRect(WORD wAnimObjId, UINT nGroupId, CRect rcStart, CRect rcFinish, UI_ANIMATION_SECONDS nDuration);
	CAnimationRect* DefineAnimationRect(WORD wAnimObjId, UINT nGroupId, CRect* pRects, int nCount, UI_ANIMATION_SECONDS nDuration);

	CAnimationValue* DefineAnimationValue(WORD wAnimObjId, UINT nGroupId);
	CAnimationValue* DefineAnimationValue(WORD wAnimObjId, UINT nGroupId, int nStart, int nEnd, UI_ANIMATION_SECONDS nDuration);





	CAnimationBaseObject* GetAnimationObjectById(WORD wObjectId);

	// Animation Sequence
	BOOL DefineSequence(WORD wSeqId, UINT nStartGroupId, UINT nEndGroupId);
	BOOL AnimateSequence(WORD wSeqId, int nRepeat = 1);
	void CleanupSequenceGroups(WORD wSeqId);

	//extra vars
	void SetPointExtraVar(WORD wId, CPoint pt);
	CPoint* GetPointExtraVar(WORD wId);

	void SetRectExtraVar(WORD wId, CRect rc);
	CRect* GetRectExtraVar(WORD wId);

	//triggers
	void TriggerAnimation(WORD wGrpId, WORD wTriggeredAnimGrp,BOOL bOn = TRUE);
	CList<DWORD, DWORD&>* GetTriggerList(WORD wGrpId);

	

	virtual void Cleanup();

private:

	// definition
	CMapWordToPtr					  m_MapIdObjToAnimObj;
	CMap<WORD, WORD&, DWORD, DWORD&>  m_SequenceDefs; // seqID = startGrp|finishGrp
	CMap<UINT, UINT&, WORD, WORD&>	  m_mapGrpToSeq; // grp => SeQ

	CMapWordToPtr					  m_mapToTriggers;

	// playing
	CMap<WORD, WORD&, int, int&>	  m_ExecutedAnimationLoops_;// seqId is a loop => nRepeatCount (si  > 1)

	//variables supplementaires
	CMapWordToPtr	  m_ptExtraVars;
	CMapWordToPtr	  m_rcExtraVars;

	BOOL			  m_bIsPaused;

};


