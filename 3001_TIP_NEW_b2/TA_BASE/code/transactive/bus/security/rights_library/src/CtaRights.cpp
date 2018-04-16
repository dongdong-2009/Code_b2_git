/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_library/src/CtaRights.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This interface is used to determine if a particular session is allowed to perform an action
  * on a resource.
  */

#include "CtaRights.h"
#include "RightsLibrary.h"

#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Bus
{
    CtaRights::CtaRights()
    {
        RightsLibraryFactory libraryFactory;
        m_rightsLibrary = std::auto_ptr<RightsLibrary>(libraryFactory.buildRightsLibrary());
        
        TA_ASSERT(m_rightsLibrary.get() != NULL, "The rights library creation failed");
    }


    CtaRights::~CtaRights()
    {
        //Empty
    }


	bool CtaRights::isActionPermittedOnResource(const std::string& session,
                                                unsigned long resource, 
                                                unsigned long action, 
                                                std::string& reason)
    {
        TA_ASSERT(m_rightsLibrary.get() != NULL, "The rights library has not been created yet");
        
        EDecisionModule decisionModule;
        return m_rightsLibrary->isActionPermittedOnResource( session,
                                                             resource,
                                                             action,
                                                             reason,
                                                             decisionModule );
    }


    bool CtaRights::isActionPermittedOnResourceAndLocSub( const std::string& session,
                                                          unsigned long resource,
                                                          unsigned long location,
                                                          unsigned long subsystem,
                                                          unsigned long action,
                                                          std::string& reason )
    {
        TA_ASSERT(m_rightsLibrary.get() != NULL, "The rights library has not been created yet");
        
        EDecisionModule decisionModule;
        return m_rightsLibrary->isActionPermittedOnResourceAndLocSub( session,
                                                                      resource,
                                                                      location,
                                                                      subsystem,
                                                                      action,
                                                                      reason,
                                                                      decisionModule );
    }

	bool CtaRights::isActionPermittedOnResourceAndLocSubWithoutDuty( const std::string& session,
                                                          unsigned long resource,
                                                          unsigned long location,
                                                          unsigned long subsystem,
                                                          unsigned long action,
                                                          std::string& reason )
    {
        TA_ASSERT(m_rightsLibrary.get() != NULL, "The rights library has not been created yet");
        
        EDecisionModule decisionModule;
        return m_rightsLibrary->isActionPermittedOnResourceAndLocSubWithoutDuty( session,
                                                                      resource,
                                                                      location,
                                                                      subsystem,
                                                                      action,
                                                                      reason,
                                                                      decisionModule );
    }

}

    