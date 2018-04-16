/*
 * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
 *
 * Source : $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssRuleProcessor.h $
 * @author  Huang Qi
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2017/12/12 14:18:48 $
 * Last modified by : $Author: hoa.le $
 *
 * 
 */

#ifndef DSSRULEPROCESSOR_H
#define DSSRULEPROCESSOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_agent/src/DssKnowledgeBase.h"

namespace TA_Base_App
{

class DssRuleProcessor
{
public:
    DssRuleProcessor();
    virtual ~DssRuleProcessor();

    void init();
    void uninit();
    void getTypeAndTriggerKeys( TypeToTriggerKeysMap& refOutput );

    unsigned long getPlanIdByTriggerKey( const unsigned long ulTriggerKey );
    void makeDecision( const unsigned long ulTriggerKey, const unsigned long ulTimestamp, const bool bCurActive,
        const bool bValue, const bool bInitial, unsigned long& ulPlanId, bool& bActivate, bool& bDeactivate );

private:
	DssRuleProcessor( const DssRuleProcessor& );
	const DssRuleProcessor& operator=( const DssRuleProcessor& );

private:
    DssKnowledgeBase m_dssKnowledgeBase;
};

} // namespace TA_Base_App

#endif // DSSRULEPROCESSOR_H
