/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/EmergencyProposalEngine.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//
//  EmergencyProposalEngine.h
//  Implementation of the Class EmergencyProposalEngine
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(EmergencyProposalEngine__INCLUDED_)
#define EmergencyProposalEngine__INCLUDED_

#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"

class ItaEcsCachedMaps;
class ItaEcsCachedConfig;
class ItaEcsAlarms;
class ItaEcsAgentCompleteState;




class EmergencyProposalEngine : public virtual AbstractFailoverable,
								public virtual IEmergencyObserver
{

public:
    EmergencyProposalEngine(
		ItaEcsCachedMaps& theMaps,
        ItaEcsCachedConfig& theConfig,
        ItaEcsAgentCompleteState& theCompleteState,
		ItaEcsDatapoints& theDatapoints);

    virtual ~EmergencyProposalEngine();

	// AbstractFailoverable overrides
    //virtual void setControl(bool isOnStartup=false);
    //virtual void setMonitor(bool isOnStartup=false);

	virtual void processEmergencyUpdate(unsigned long zoneId, bool value);

	ItaEcsAgentCompleteState& getCompleteState();


private:

	EmergencyProposalEngine(const EmergencyProposalEngine& theEmergencyProposalEngine);

	void addMasterModeToProposals(unsigned long masterMode);

	void addRemoveNormalModeToStationIfRequired();

private:

	ItaEcsCachedMaps&                           m_theMaps;
    ItaEcsCachedConfig&                         m_theConfig;
    ItaEcsAgentCompleteState&                   m_theCompleteState;
	ItaEcsDatapoints&							m_theDatapoints;
    

};

#endif // !defined(EmergencyProposalEngine__INCLUDED_)
