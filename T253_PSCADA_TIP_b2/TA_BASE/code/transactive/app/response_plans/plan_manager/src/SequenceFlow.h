//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/SequenceFlow.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2014/05/26 14:33:17 $
// Last modified by:  $Author: huangjian $
//
// <description>

#ifndef SEQUENCEFLOW_H_INCLUDED
#define SEQUENCEFLOW_H_INCLUDED

#include <string>

namespace TA_Base_App
{

class PlanNode;

class SequenceFlow
{
// Operations
public:
    SequenceFlow( PlanNode* parent, const unsigned int unId );  // For new sequence flow
    SequenceFlow( PlanNode* parent, const unsigned int pkey, const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing, const std::string& strCondition, const std::string& strGraph );

    virtual ~SequenceFlow();

    // Accessory methods
    unsigned int pkey() const;
    unsigned int id() const;
    unsigned int incomingStep() const;
    unsigned int outgoingStep() const;
    std::string condition() const;
    std::string graph() const;

    // Editor methods
    void incomingStep( const unsigned int stepId );
    void outgoingStep( const unsigned int stepId );
    void id( const unsigned int stepId );
    void pkey( const unsigned int pkey );
    void condition( const std::string& condition );
    void graph( const std::string& graph );

    // Methods to get updated step information for writing back to agent
    PlanNode* getParentPlan() const { return m_parent; }

protected:
    SequenceFlow(const SequenceFlow& SequenceFlow);
    SequenceFlow &operator=(const SequenceFlow &other);

// Attributes
protected:
    PlanNode* m_parent;
    unsigned int m_unPkey;
    unsigned int m_unId;
    unsigned int m_unIncomingStep;
    unsigned int m_unOutgoingStep;
    std::string m_strCondition;
    std::string m_strGraph;
};

}

#endif // !defined(SEQUENCEFLOW_H_INCLUDED)
