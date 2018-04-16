#ifndef IACTIVEPLANFLOW_H_INCLUDED
#define IACTIVEPLANFLOW_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2014/11/04 13:46:22 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include <map>
#include <vector>
#include <string>

namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_App
{
	//Class declaration
	class IActivePlan
	{
	// Public methods
	public:
        // Step actions
        virtual bool stepCompleted( const unsigned long ulStepNum ) = 0;
        virtual bool allStepPaused( const bool bIgnoreParallels ) = 0;

        virtual void planSharedData( const std::string& strName, const std::string& strValue ) = 0;
        virtual void planSharedData( const std::map<std::string, std::string>& mapData ) = 0;
        virtual const std::map<std::string, std::string> planSharedData() = 0;
        virtual const std::string planSharedData( const std::string& strName ) = 0;

        virtual void stepResponsed( const unsigned long ulStepNum, const int nResponse ) = 0;
        virtual const int stepResponsed( const unsigned long ulStepNum, const bool bErase = true ) = 0;

        virtual void stepData( const unsigned long ulStepNum, const std::vector<std::string>& strData ) = 0;
        virtual std::vector<std::string> stepData( const unsigned long ulStepNum, const bool bErase = true ) = 0;

        // Plan information
        virtual const TA_Base_Core::ActivePlanId& id() const = 0;
        virtual const std::string name() const = 0;
        virtual const std::string session() const = 0;
        virtual const unsigned int userLocation() const = 0;
        virtual const unsigned int userWorkstation() const = 0;

	//private:
	//	IActivePlanFlow& operator=( const IActivePlanFlow& );
	//	IActivePlanFlow( const IActivePlanFlow& );

	}; // IActivePlanFlow
}; // TA_Base_App

#endif // IACTIVEPLANFLOW_H_INCLUDED
