/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/StepImplementationFactory.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #16 $
  *
  *  Last modification: $DateTime: 2015/10/26 13:03:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#ifndef STEP_IMPLEMENTATION_FACTORY_H
#define STEP_IMPLEMENTATION_FACTORY_H

#include <map>
#include <string>
#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "ace/DLL.h"

namespace TA_Base_App
{

    class IActiveStepMessage;
    class IStepComponent;
    class IActivePlan;
    class ActiveStep;
    class BasicParameter;

    typedef IStepComponent* (* GetStepComponentPtr) ();
    typedef std::map<long, IStepComponent*> StepComponentMap;

    class ActiveStepFactoryImpl
    {
    public:
        friend class ACE_Singleton<ActiveStepFactoryImpl, ACE_Recursive_Thread_Mutex>;
        ActiveStep* createStep( const BasicParameter& detail, IActivePlan& plan );
        IActiveStepMessage* createStepMessage( const int stepType, const unsigned long ulStepNum, const std::vector<std::string>& parameters );
        void validateStepParameter( const int stepType, const std::vector<std::string>& parameters );
    
    private:
        ActiveStepFactoryImpl();
        ~ActiveStepFactoryImpl();
        ActiveStepFactoryImpl( const ActiveStepFactoryImpl& );
        ActiveStepFactoryImpl& operator=( const ActiveStepFactoryImpl& );

    private:
		std::map<long, ACE_DLL> m_stepDllMap;
        StepComponentMap m_stepComponentMap;
    };

    typedef ACE_Singleton<ActiveStepFactoryImpl, ACE_Recursive_Thread_Mutex> ActiveStepFactory;
}

#endif