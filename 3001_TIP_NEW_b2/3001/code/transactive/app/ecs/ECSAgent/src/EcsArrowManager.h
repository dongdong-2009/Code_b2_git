/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/EcsArrowManager.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//
//  EcsArrowManager.h
//  Implementation of the Class EcsArrowManager
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(EcsArrowManager__INCLUDED_)
#define EcsArrowManager__INCLUDED_


#include "app/ecs/ECSAgent/src/EcsGlobals.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

class ItaEcsCachedMaps;


//TD16709
enum ArrowUpdateType{
	ALL,
	SELECTED,
	APPLIED
};

class EcsArrowManager : public virtual AbstractFailoverable,
						public virtual IEmergencyObserver
{

public:
    EcsArrowManager(ItaEcsCachedMaps& theMaps, ItaEcsDatapoints& theDatapoints);

    virtual ~EcsArrowManager();

    void setSelectedMasterMode(unsigned long masterMode);
    void setAppliedMasterMode(unsigned long masterMode);

    // AbstractFailoverable overrides
    virtual void setControl(bool isOnStartup=false);
    virtual void setMonitor(bool isOnStartup=false);

	virtual void processEmergencyUpdate(unsigned long zoneId, bool value);

private:
    EcsArrowManager( const EcsArrowManager& theEcsArrowManager);
    EcsArrowManager();


private:
    
    unsigned long convertToArrowsEnum(char displayDirection);
    
    void processDisplayDirection(const char* displayDirection, 
                                 unsigned long& evacuationState, 
                                 unsigned long& airflowState);

    void updateAllArrows(ArrowUpdateType updateType);  //TD16709
	void arrowsInitialise();  //TD16709


    ItaEcsCachedMaps&                               m_theMaps;
    ItaEcsDatapoints&                               m_theDatapoints;

	unsigned long									m_currentSelectedMasterMode;
	unsigned long									m_currentAppliedMasterMode;

    TA_Base_Core::ReEntrantThreadLockable           m_lock;

};

#endif // !defined(EcsArrowManager__INCLUDED_)
