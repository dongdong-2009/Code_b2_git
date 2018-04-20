//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/FloatingDocTemplate.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
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
