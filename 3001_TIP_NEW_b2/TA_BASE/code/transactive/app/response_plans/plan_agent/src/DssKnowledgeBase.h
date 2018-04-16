/*
 * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/DssKnowledgeBase.h $
 * @author  Huang Qi
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/12/31 10:15:37 $
 * Last modified by : $Author: huangjian $
 *
 * 
 */

#ifndef DSSKNOWLEDGEBASE_H
#define DSSKNOWLEDGEBASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

namespace TA_Base_App
{

enum DssRuleType
{
    DssRuleTypeInvalid = 0,
    DssRuleTypeDatapoint,
    DssRuleTypeCount
};

typedef std::map< int, std::vector<unsigned long> > TypeToTriggerKeysMap;

class IDssRule;
class DssAbstractRule;

class DssKnowledgeBase
{
public:
    DssKnowledgeBase();
    virtual ~DssKnowledgeBase();

    void init();
    void uninit();
    void getTypeAndTriggerKeys( TypeToTriggerKeysMap& refOutput );

    IDssRule& getDssRuleByDatapointKey( const unsigned long ulKey );

private:
	DssKnowledgeBase( const DssKnowledgeBase& );
	const DssKnowledgeBase& operator=( const DssKnowledgeBase& );

private:
    typedef std::map<unsigned long, DssAbstractRule*> KeyToRuleMap;
    typedef std::map<int, KeyToRuleMap> TypeToRulesMap;
    TypeToRulesMap m_mapDssRules;
};

} // namespace TA_Base_App

#endif // DSSKNOWLEDGEBASE_H
