#ifndef PLANACCESSFACTORY_H_INCLUDED
#define PLANACCESSFACTORY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAccessFactory.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanAccessFactory" and that "PlanAccessFactoryImpl" can
	// not be used.
	class PlanAccessFactoryImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<PlanAccessFactoryImpl, ACE_Recursive_Thread_Mutex>;

		// A list of steps that can be executed
        ActiveStepList* getActiveStepList( TA_Base_Core::NodeId plan, IActivePlanFlow& flowControl, std::string& reason ) const;
		bool isGRCType3Step(const TA_Base_Core::StepDetail& stepDetail) const;
	
	// Protected methods
	// Private methods
	private:
		PlanAccessFactoryImpl();
		~PlanAccessFactoryImpl(){}

		PlanAccessFactoryImpl( const PlanAccessFactoryImpl& );
		PlanAccessFactoryImpl& operator=( const PlanAccessFactoryImpl& );

	}; // PlanAccessFactoryImpl
	typedef ACE_Singleton<PlanAccessFactoryImpl, ACE_Recursive_Thread_Mutex> PlanAccessFactory;
} // TA_Base_App

#endif //PLANACCESSFACTORY_H_INCLUDED
