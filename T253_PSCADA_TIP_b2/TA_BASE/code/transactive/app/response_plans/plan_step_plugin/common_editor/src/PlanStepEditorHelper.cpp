
#include <sstream>
#include <iomanip>

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditorHelper.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanNodeEditorHelper.h"

namespace TA_Base_App
{

const unsigned int PlanStepEditorHelper::INVALID_ID = -1;

PlanStepEditorHelper::PlanStepEditorHelper(IPlanNode& parent) : 
m_itemMap(),
m_planNode(parent),
m_mtxItemMap(),
m_bInitilized( false )
{
}

PlanStepEditorHelper::~PlanStepEditorHelper()
{
}

std::vector<ListItem> PlanStepEditorHelper::getSteps()
{
	initHelper();
    
    ACE_Read_Guard<ACE_RW_Mutex> gudRead( m_mtxItemMap );

	std::vector<ListItem> items;

	for ( StepItemMap::iterator itLoop = m_itemMap.begin(); m_itemMap.end() != itLoop; ++itLoop )
	{
        items.push_back( itLoop->second );
	}

	return items;
}

ListItem PlanStepEditorHelper::getItemById( unsigned int id )
{
	initHelper();
    
    ACE_Read_Guard<ACE_RW_Mutex> gudRead( m_mtxItemMap );

	ListItem item = {INVALID_ID, ""};

    StepItemMap::const_iterator itFound = m_itemMap.find( id );
	if( m_itemMap.end() != itFound )
	{
        item = itFound->second;
	}

	return item;
}

void PlanStepEditorHelper::initHelper()
{
    if ( m_bInitilized )
    {
        return;
    }

    ACE_Write_Guard<ACE_RW_Mutex> gudWrite( m_mtxItemMap );
    
    // Double boolean with lock
    if ( m_bInitilized )
    {
        return;
    }

	m_itemMap.clear();
    std::vector<PlanStep*> planSteps = m_planNode.getPlanSteps();
    std::stringstream stmFormat;

	for ( std::vector<PlanStep*>::iterator itLoop = planSteps.begin(); planSteps.end() != itLoop; ++itLoop )
	{
        PlanStep* pStep = *itLoop;

        if ( NULL == pStep )
        {
            continue;
        }

        stmFormat.clear();
        stmFormat.str( "" );
        stmFormat << std::setw( 2 ) << std::setfill( '0' ) << pStep->getStepPosition() << ":" << pStep->getStepTypeName() << " - " << pStep->getStepName();

        ListItem item = { pStep->getStepId(), "" };
        StepItemMap::iterator itInserted = m_itemMap.insert(std::make_pair(item.ID, item)).first;

        if ( m_itemMap.end() != itInserted )
        {
            itInserted->second.Item = stmFormat.str().c_str();
        }
	}

    // Always re-init in edit mode
    m_bInitilized = !m_planNode.isCopiedPlan();
}


}
