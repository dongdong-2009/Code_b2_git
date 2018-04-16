/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/plans/src/PlanAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * PlanAccessFactory is a singleton that is used to retrieve IPlan objects.
  */

#ifdef __WIN32__
#pragma warning (disable:4786)
#endif

#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/plans/src/PlanDAI.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::ReEntrantThreadLockable PlanAccessFactory::m_singletonLock;
	PlanAccessFactory* PlanAccessFactory::s_instance = NULL;


    PlanAccessFactory& PlanAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new PlanAccessFactory;
        }
        return *s_instance;
    }


    PlanAccessFactory::~PlanAccessFactory()
    {

    }


    IPlan* PlanAccessFactory::getPlanByPath(const std::string& planPath) const
    {
        // Construct a new plan with the path we have
        IPlan* plan = new PlanDAI(planPath);

        // Force it to load the details so we know if this path is valid or not. 
        // If this path is not valid, this call will throw an exception
        try
        {
            plan->getPath();
        }
        catch ( ... )
        {
            delete plan;
            throw;
        }

        return plan;
    }


    std::string PlanAccessFactory::getPlanName(const std::string& planPath) const
    {
        const std::string CATEGORY_SEPARATOR = "/";
        std::string::size_type sepPos(planPath.rfind(CATEGORY_SEPARATOR));

        return (sepPos != std::string::npos? planPath.substr(sepPos + 1) : planPath);
    }


} //end namespace TA_Base_Core


