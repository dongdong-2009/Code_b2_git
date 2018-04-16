/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef BROADCASTMODEL_H__INCLUDED_
#define BROADCASTMODEL_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ICurSelDVAMsgObserver;
class IAddDVAMsgListObserver;

class PaRuntimeModel
{
typedef std::pair<unsigned long, DVAMsgInfo*> 	DVAMsg;
typedef std::vector<DVAMsg> 					DVAMsgList;

public:
    PaRuntimeModel();
    virtual ~PaRuntimeModel();

public:
    std::string getCurDVAMsgName( int nType );
    std::string getCurDVAMsgDesc( int nType );
    int modifyCurDVAMsgDesc( int nType, const std::string& strDesc );
    void clearCurSelDVAMsg( int nType, bool bForceNotify = false );
    void updateCurSelDVAMsg( int nType, unsigned long ulDVAMsgKey, bool bForceNotify = false );
    DVAMsgInfo* getCurSelDVAMsg( int nType );
    DVAMsgInfo* getActiveSelDVAMsg();
    unsigned long getActiveSelDVAMsgKey();

	//Process play DVA message.
	//From PaDVAMsgListCtrl
	bool addToPlayDVAMsgList(int nType, unsigned long ulDVAMsgKey);
	void getPlayDVAMsgList(std::vector<unsigned long>& vecMsgs);
	void getPlayDVAMsgInfo(unsigned long ulDVAMsgKey,
		std::string& strID,
		std::string& strName);
	//From PlayDVAMsgListCtrl to update the sequence of the list.
	void upDVAMsgInList(unsigned long ulPosition);
	void downDVAMsgInList(unsigned long ulPosition);
	void deleteDVAMsgInList(unsigned long ulPosition);

    void registerAddDVAMsgListObserver( IAddDVAMsgListObserver* pObserver );
    void deregisterAddDVAMsgListObserver( IAddDVAMsgListObserver* pObserver );
	
	
    // Current selection DVA message observer function
    void registerCurSelDVAMsgObserver( int nType, ICurSelDVAMsgObserver* pObserver );
    void deregisterCurSelDVAMsgObserver( int nType, ICurSelDVAMsgObserver* pObserver );

    void registerZoneSelectionNotify( HWND hWnd );
    void deregiseterZoneSelectionNotify();

    std::vector<unsigned long> getActiveSelZoneKeysList();
    void getActiveSelZoneNames( std::vector<std::string>& vecZoneNames );
    void updateCurSelZoneGroupKey( unsigned long ulKey );
    void getCurZones( std::vector<unsigned long>& vecZones );

    bool isSelectedZoneEmpty();
    bool checkCyclicTimesRange( int nCyclicTimes );

    void lockZoneList( bool bLock = true );

private:
    // Disable copy constructor and operator=
    const PaRuntimeModel& operator=( const PaRuntimeModel& );
    PaRuntimeModel( const PaRuntimeModel& );

protected:
    HmiSelectionListener* m_pHMISelectionListener;

    DVAMsg		m_pirCurSelEmergencyDVAMsg;
    DVAMsg 		m_pirCurSelNormalDVAMsg;
    DVAMsg 		m_pirCurSelOtherDVAMsg;
    DVAMsg 		m_pirActiveSelDVAMsg;
	DVAMsgList  m_playDVAMsgList;

    std::vector<ICurSelDVAMsgObserver*> m_vecEmergencyCurSelDVAMsgObservers;
    std::vector<ICurSelDVAMsgObserver*> m_vecNormalCurSelDVAMsgObservers;
    std::vector<ICurSelDVAMsgObserver*> m_vecOtherCurSelDVAMsgObservers;
    std::vector<IAddDVAMsgListObserver*> m_vecAddDVAMsgListObservers;

	
    // Auto update
    unsigned long 						m_ulCurSelZoneGroupKey;
    bool 								m_bZoneListLocked;
    std::vector<unsigned long> 			m_lstCachedZoneList;
};

//{{AFX_INSERT_LOCATION}}

#endif // ifndef BROADCASTMODEL_H__INCLUDED_

