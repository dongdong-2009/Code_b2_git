//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/FloatingDocTemplate.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>

#if !defined(AFX_FLOATINGDOCTEMPLATE_H__764A92E9_628A_4ED1_942C_9DE4A380431D__INCLUDED_)
#define AFX_FLOATINGDOCTEMPLATE_H__764A92E9_628A_4ED1_942C_9DE4A380431D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class PlanNode;


    class CFloatingDocTemplate : public CSingleDocTemplate
    {
    public:
        CFloatingDocTemplate(UINT                    nIDResource,
                             CRuntimeClass*            pDocClass,
                             CRuntimeClass*            pFrameClass,
                             CRuntimeClass*            pViewClass,
                             TA_Base_App::PlanNode*      planNode);
        virtual ~CFloatingDocTemplate();

        PlanNode* getNode();


    private:
        PlanNode*  m_planNode;
    };
}

#endif // !defined(AFX_FLOATINGDOCTEMPLATE_H__764A92E9_628A_4ED1_942C_9DE4A380431D__INCLUDED_)
