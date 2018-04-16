#include "app/response_plans/plan_step_plugin/common_editor/src/PlanNodeEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "bus/response_plans/plan_tree/src/AgentPlanAccess.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{

PlanNodeEditorHelper::PlanNodeEditorHelper() : m_planAccess(NULL)
{
    std::vector<TA_Base_Core::NodeId> nonFilterablePlans;
    try
    {
        m_planAccess = new TA_Base_Bus::AgentPlanAccess(TA_Base_Bus::PlanFilter::PFT_APPROVED_PLANS, nonFilterablePlans);
    }
    catch(...)
    {
        // TODO
    }
}

PlanNodeEditorHelper::~PlanNodeEditorHelper()
{
    delete m_planAccess;
}

std::vector<TreeItem> PlanNodeEditorHelper::getRootItems()
{
	std::vector<TreeItem> items;
    TreeItem root = {true, 1, 1, "Plans"}; // For root item, id == parentid
	items.push_back(root);
    return items;
}

std::vector<TreeItem> PlanNodeEditorHelper::getChildItems(unsigned long nodeId)
{
    std::vector<TreeItem> items;
    if(NULL == m_planAccess)
    {
        return items;
    }

    TA_Base_Bus::IPlanAccess::PlanNodeList children;
    try
    {
        if(1 == nodeId)
        {
            children = m_planAccess->getRootChildren();
        }
        else
        {
            children = m_planAccess->getChildren(nodeId);
        }
    }
    catch(...)
    {
    }
    BOOST_FOREACH(TA_Base_Core::NodeSummary & i, children)
    {
        TreeItem item = {i.hasChildren, i.node, nodeId, i.name.in()};
        items.push_back(item);
    }
    return items;
}

std::string PlanNodeEditorHelper::getPlanPath(unsigned long planNodeId)
{
    if(NULL == m_planAccess)
    {
        return "";
    }
    try
    {
        return m_planAccess->getPlanPath(planNodeId);
    }
    catch(...)
    {
        // TODO
    }
    return "";
}

}