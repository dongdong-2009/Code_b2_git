/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/SequenceFlowMap.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * This class is a container used by a PlanNode to store its PlanStep's. The steps are stored in a map
  * where the key is their step ID. This class allows steps to be added, deleted, copied and moved.
  *
  **/

#ifndef SEQUENCEFLOWMAP_H__INCLUDED_
#define SEQUENCEFLOWMAP_H__INCLUDED_

#include <map>
#include <set>
#include <vector>

#include "ace/RW_Mutex.h"

namespace TA_Base_App
{

class SequenceFlow;
class PlanNode;

typedef std::vector<SequenceFlow*> SequenceFlowList;

class SequenceFlowMap
{
// Operations
public:
    SequenceFlowMap(PlanNode* parent);
    virtual ~SequenceFlowMap();

    void getSequenceFlows( SequenceFlowList& planSteps );
    SequenceFlow* getSequenceFlow( const unsigned int unId );
    void getSequenceFlowId( const unsigned int unIncoming, const unsigned int unOutgoing, unsigned int& unFlowId, bool& bFound );
    void getSequenceFlows( const unsigned int unStepId, std::map<unsigned int, unsigned int>& mapFlowIdToIncoming, std::map<unsigned int, unsigned int>& mapFlowIdToOutgoing );
    void getSequenceFlows( const unsigned int unStepId, std::vector<unsigned int>& vecIncoming, std::vector<unsigned int>& vecOutgoing );
    
    void insertSequenceFlow( const unsigned int unId );
    void insertSequenceFlow( const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing );
    void insertSequenceFlow( const unsigned int pkey, const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing, const std::string& strCondition, const std::string& strGraph );
    void deleteSequenceFlow( const unsigned int unId );
    void deleteSequenceFlow( const unsigned int unIncoming, const unsigned int unOutgoing );
    void clearSequenceFlow();

    bool linkFlowFrom( const unsigned int unId, const unsigned int unIncoming, unsigned int& unPrevIncoming, unsigned int& unCurOutgoing, const bool bResetGraph = false );
    bool linkFlowTo( const unsigned int unId, const unsigned int unOutgoing, unsigned int& unPrevOutgoing, unsigned int& unCurIncoming, const bool bResetGraph = false );
    void linkFlowFrom( const unsigned int unId, const unsigned int unIncoming, const bool bResetGraph = false );
    void linkFlowTo( const unsigned int unId, const unsigned int unOutgoing, const bool bResetGraph = false );

    size_t countSequenceFlow( const unsigned int unIncoming, const unsigned int unOutgoing );

    unsigned long getNewId();

protected:
    SequenceFlow* createSequenceFlow( const unsigned int pkey, const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing, const std::string& strCondition, const std::string& strGraph );
    SequenceFlow* createNewSequenceFlow( unsigned int unId );
    void insertSequenceFlow( SequenceFlow& refFlow );

private:
    SequenceFlowMap(const SequenceFlowMap &other);
    SequenceFlowMap &operator=(const SequenceFlowMap &other);

// Attributes
private:
    ACE_RW_Mutex m_mtxDataLock;
    std::map<unsigned int, SequenceFlow*> m_mapSequenceFlows;
    std::map<unsigned int, std::set<SequenceFlow*>> m_linkedSequcenceFlows;

    PlanNode* m_parent;
};

}

#endif // !defined(SEQUENCEFLOWMAP_H__INCLUDED_)
