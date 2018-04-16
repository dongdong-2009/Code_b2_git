/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/response_plans/plan_tree/src/PlanItem.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// PlanItem.h : header file
//

#if !defined(AFX_PLANITEM_H__87B9CD26_7C13_4C51_A315_32820D72E524__INCLUDED_)
#define AFX_PLANITEM_H__87B9CD26_7C13_4C51_A315_32820D72E524__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/object_tree_control/ObjectTreeItem.h"
#include "bus/response_plans/plan_tree/src/IPlanAccess.h"


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanItem

    class CPlanItem : public CObjectTreeItem
    {
    // Construction/destruction
    public:
	    CPlanItem(const TA_Base_Core::NodeSummary& nodeSummary, IPlanAccess& planAccess);
	    virtual ~CPlanItem();

    private:
        CPlanItem(const CPlanItem& planItem);


    // Operations
    public:
        TA_Base_Core::NodeId getPlanId();
        std::string getPlanPath();

        // CObjectTreeItem overrides
        virtual bool createChildItems();
        virtual const char *getItemLabelText();
        virtual bool hasChildren();
        virtual const std::string getKey();
        virtual HICON getItemImage(bool itemExpanded);

    private:
        CPlanItem& operator=(const CPlanItem& planItem);


    // Attributes
    private:
        TA_Base_Core::NodeSummary m_nodeSummary;
        IPlanAccess& m_planAccess;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_PLANITEM_H__87B9CD26_7C13_4C51_A315_32820D72E524__INCLUDED_)
