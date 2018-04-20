/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/CongestionProposalEngine.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  CongestionProposalEngine.h
//  Implementation of the Class CongestionProposalEngine
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(CongestionProposalEngine__INCLUDED_)
#define CongestionProposalEngine__INCLUDED_

#include "app/ecs/ECSAgent/src/EcsGlobals.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/EcsZoneCongestionContext.h"
#include "app/ecs/ECSAgent/src/EcsGlobals.h"

#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"


class ItaEcsCachedConfig;
class ItaEcsAgentCompleteState;

struct EvaluationDetail
{
    EvaluationDetail()
    : m_expression (0), m_stationMode (TA_IRS_Bus::ECS_NONE_OR_INVALID)
    { }

    TA_Base_Bus::ExpressionEvaluator*	m_expression;
    unsigned long						m_stationMode;
};


class CongestionProposalEngine : public virtual AbstractFailoverable,
                                 public virtual ICongestionObserver
{

public:

    CongestionProposalEngine(
    	ItaEcsCachedConfig& theConfig, 
        ItaEcsCachedMaps& theMaps, 
        ItaEcsDatapoints& theDatapoints,
        ItaEcsAgentCompleteState& theCompleteState);

    virtual ~CongestionProposalEngine();

    // AbstractFailoverable overrides
    virtual void setControl(bool isOnStartup=false);
    virtual void setMonitor(bool isOnStartup=false);

    virtual void processCongestionUpdate(unsigned long zoneId, unsigned long value);

private:

	CongestionProposalEngine( const CongestionProposalEngine& theCongestionProposalEngine);

    void refreshMaps();
	void cleanUpMaps();
    void processProposalsForLocation(unsigned long location, unsigned long locationKeyOfCongestionPair);
	unsigned long getNextZoneId(std::string& congestionLogic, unsigned long& lastFoundPos);


private:

    typedef std::multimap<unsigned long, EvaluationDetail>  LocationToEvaluationDetailsMap;
    typedef LocationToEvaluationDetailsMap::iterator        LocationToEvaluationDetailsMapIt;
	typedef LocationToEvaluationDetailsMap::value_type      Location2EvaluationDetail;

    typedef std::multimap<unsigned long, unsigned long>     ZoneToLocationsMap;
    typedef ZoneToLocationsMap::iterator                    ZoneToLocationsMapIt;
	typedef ZoneToLocationsMap::value_type					Zone2Location;

    ItaEcsCachedConfig&                             m_theConfig;
    ItaEcsCachedMaps&                               m_theMaps;
    ItaEcsDatapoints&                               m_theDatapoints;
    ItaEcsAgentCompleteState&                       m_theCompleteState;

    TA_Base_Core::ReEntrantThreadLockable           m_lockForConfigUpdates;
    LocationToEvaluationDetailsMap                  m_locationEvaluationDetails;
    ZoneToLocationsMap                              m_zoneLocations;
	EcsZoneCongestionContext						m_booleanContext;

};

#endif // !defined(CongestionProposalEngine__INCLUDED_)