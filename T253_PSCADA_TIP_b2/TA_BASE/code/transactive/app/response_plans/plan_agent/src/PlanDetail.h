#ifndef PLANDETAIL_H_INCLUDED
#define PLANDETAIL_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanDetail.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2017/12/12 14:18:48 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#include <string>

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{

typedef std::map<unsigned long, TA_Base_Core::ActiveStepDetail> ExeHistoryMap;
typedef std::map<unsigned long, TA_Base_Core::ActiveStepDetail>::iterator ExeHistoryMapIt;

typedef std::map<unsigned long, TA_Base_Core::StepCustomisableDetail> CustHistoryMap;
typedef std::map<unsigned long, TA_Base_Core::StepCustomisableDetail>::iterator CustHistoryMapIt;

typedef std::map<std::string, std::string> KeyValueMap;
typedef std::map<std::string, std::string>::iterator KeyValueMapIt;

class PlanDetail
{
public:
    PlanDetail( const PlanDetail& );
    PlanDetail( const std::string& session, const unsigned long triggerEntity, const unsigned long userKey, const unsigned long userLocation,
        const unsigned long userWrokstation, const unsigned long updateId, const std::string& owner, const TA_Base_Core::ActivePlanId& refActivePlan, 
        const bool bPending, const std::string& remark, const bool manExecuted, const std::string& assocAlarmId );

    bool set( const int state );

	TA_Base_Core::ActivePlanDetail detail;
	//note detail.timeout is reset for each step in a plan

    //alarm ID of the alarm raised by this Plan
    std::string alarmID;

	unsigned long userKey;
    unsigned long userLocation;
    unsigned long userWorkstation;

    ExeHistoryMap exeHistoryMap;
    CustHistoryMap custHistoryMap;
    KeyValueMap sharedData;

	time_t lastModified;

	bool isNonActive() const;
    bool isActive() const;

private:
    PlanDetail();
	PlanDetail& operator=( const PlanDetail& );
};

} // TA_Base_App

#endif //PLANDETAIL_H_INCLUDED
