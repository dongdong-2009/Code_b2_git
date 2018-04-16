/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FloatingWindowThreadRegister.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Activates and stores Plan Controller and Plan Editor threads.
  *
  **/

#if !defined(AFX_FLOATINGWINDOWTHREADREGISTER_H__E4016AC0_F3C0_4EAA_BC29_6E2CD9B90D6D__INCLUDED_)
#define AFX_FLOATINGWINDOWTHREADREGISTER_H__E4016AC0_F3C0_4EAA_BC29_6E2CD9B90D6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "PlanManagerCommonDefs.h"
#include "SynchronisedMap.h"


namespace TA_Base_App
{
    class CFloatingWindowThread;
    class CPlanControllerThread;
    class CPlanEditorThread;


    /////////////////////////////////////////////////////////////////////////////
    // FloatingWindowThreadRegister singleton

    class FloatingWindowThreadRegister
    {
    // Operations
    public:
        ~FloatingWindowThreadRegister();

        static FloatingWindowThreadRegister &getInstance();
        static void destroyInstance();

        CPlanControllerThread *createPlanControllerThread(TreeNodeId planId, bool ignoreActivePlanControllerLimit);
        CPlanEditorThread *createPlanEditorThread(TreeNodeId planId);

        CPlanControllerThread *getPlanControllerThread(TreeNodeId planId);
        CPlanEditorThread *getPlanEditorThread(TreeNodeId planId);

        void terminateAllThreads();
        void deregisterThread(CFloatingWindowThread *floatingWindowThread);


    private:
        FloatingWindowThreadRegister();
        FloatingWindowThreadRegister(const FloatingWindowThreadRegister &other);
        FloatingWindowThreadRegister &operator=(const FloatingWindowThreadRegister &other);

        std::vector<CPlanControllerThread *> getRegisteredPlanControllerThreads();
        std::vector<CPlanEditorThread *> getRegisteredPlanEditorThreads();


    // Attributes
    private:
        static FloatingWindowThreadRegister *s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        typedef SynchronisedMap<TreeNodeId, CPlanControllerThread *> PlanControllerThreadMap;
        typedef SynchronisedMap<TreeNodeId, CPlanEditorThread *> PlanEditorThreadMap;

        PlanControllerThreadMap m_planControllerThreadMap;
        PlanEditorThreadMap m_planEditorThreadMap;

        CEvent m_deregisterEvent;
    };
}

#endif // !defined(AFX_FLOATINGWINDOWTHREADREGISTER_H__E4016AC0_F3C0_4EAA_BC29_6E2CD9B90D6D__INCLUDED_)
