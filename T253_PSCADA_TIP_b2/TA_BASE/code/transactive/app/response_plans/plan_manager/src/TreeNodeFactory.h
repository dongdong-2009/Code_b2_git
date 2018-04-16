//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TreeNodeFactory.h $
// @author:  Bart Golab
// @version: $Revision: #6 $
//
// Last modification: $DateTime: 2015/08/31 17:30:52 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_TREENODEFACTORY_H__AB7D8100_2FE6_44D3_A8ED_07BB43925EB2__INCLUDED_)
#define AFX_TREENODEFACTORY_H__AB7D8100_2FE6_44D3_A8ED_07BB43925EB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "SynchronisedMap.h"


namespace TA_Base_App
{
    class TreeNode;
    class RootNode;


    /////////////////////////////////////////////////////////////////////////////
    // TreeNodeFactory singleton

    class TreeNodeFactory
    {
    // Operations
    public:
        static TreeNodeFactory &getInstance();
        static void destroyInstance();
        static unsigned long getRootNodeId();

        RootNode *createRootNode();
        TreeNode *createTreeNode(unsigned long nodeId);
        TreeNode *createTreeNode(const TA_Base_Core::NodeSummary &nodeSummary);
        void createChildren(unsigned long nodeId, std::vector<TreeNode*>& children);
        void createRootChildren(std::vector<TreeNode*>& children);
        void retireTreeNode(unsigned long nodeId);

        void getAllTreeNode( std::vector<TreeNode*>& refTreeNodes );
        RootNode *getRootNode();
        TreeNode *getTreeNode(unsigned long nodeId);
        TreeNode *getClosestAncestor(unsigned long nodeId);
        TreeNode *getTreeNodeOrClosestAncestor(unsigned long nodeId);
        int getTreeNodeLevelInTree(unsigned long nodeId);

        void getNodeIdsInNodePath(unsigned long nodeId, std::vector<unsigned long>& nodeIds);
        
        bool isTreeNodeVisible(unsigned long nodeId);
        bool canTreeNodeBeShowed(unsigned long nodeId);

    private:
        TreeNodeFactory();
        ~TreeNodeFactory();
        TreeNodeFactory(const TreeNodeFactory &other);
        TreeNodeFactory& operator=(const TreeNodeFactory &other);
        void createTreeNodes(const TA_Base_Core::SiblingNodes& nodes, std::vector<TreeNode*>& treeNodes);


    // Attributes
    private:
        static TreeNodeFactory* s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        SynchronisedMap<unsigned long, TreeNode*> m_treeNodeMap;

        std::map<unsigned long, int> m_planTypeMap;

    };

} // namespace TA_Base_App


#endif // !defined(AFX_TREENODEFACTORY_H__AB7D8100_2FE6_44D3_A8ED_07BB43925EB2__INCLUDED_)
