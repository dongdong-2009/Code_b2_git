#ifndef PLANINTERNETIONAL_H__INCLUDED_
#define PLANINTERNETIONAL_H__INCLUDED_

#include <string>

namespace TA_Base_Bus
{

class PlanInternetional
{
public:
    static void initLocaleSimpChinese(); // Chinese 2052 
    static void initLocaleEnglish(); // English 3081

public:
    static std::string ActivePlanListColPlan;			    // Column 1 - Plan
    static std::string ActivePlanListColCategory;	    // Column 2 - Category
    static std::string ActivePlanListColInstance;	    // Column 3 - Instance
    static std::string ActivePlanListColLocation;       // Column 4 - Location
    static std::string ActivePlanListColOwner;			    // Column 5 - Owner
    static std::string ActivePlanListColActivation;	// Column 6 - Activation time
    static std::string ActivePlanListColState;		    	// Column 7 - State
    static std::string ActivePlanListColStep;		    	// Column 8 - Step
    static std::string ActivePlanListColRemarks;		    // Column 9 - Remarks

    static std::string PlanTreeRootItem;

    static std::string ActivePlanStateRunning;
    static std::string ActivePlanStateStpped;
    static std::string ActivePlanStatePaused;
    static std::string ActivePlanStateCompleted;
    static std::string ActivePlanStatePending;
    static std::string ActivePlanStateUndefined;
};

}

#endif
