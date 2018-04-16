//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ProfileConfigAccess.cpp $
// @author:  Huang Jian
// @version: $Revision: #1 $
//
//
// This is a utility class. It provides methods for determining a data point's type and,
// where applicable, configured states.

#include "ProfileConfigAccess.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{


    ProfileConfigAccessImpl::~ProfileConfigAccessImpl()
    {
        if (!m_profileMap.empty())
        {
            ProfileMap::iterator itr = m_profileMap.begin();
            for( ;itr != m_profileMap.end(); ++itr)
            {
                delete itr->second;
                itr->second = NULL;
            }
            m_profileMap.clear();
        }
    }

    ProfileConfigAccessImpl::ProfileConfigAccessImpl()
    {
        std::vector<TA_Base_Core::IProfile *> profileVec;
        if (m_profileMap.empty())
        {
            try
            {
                profileVec = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Retrieve all profile failed", "");
            }
            
            std::vector<TA_Base_Core::IProfile*>::iterator cstItr = profileVec.begin();
            for(;cstItr != profileVec.end(); ++cstItr)
            {
                m_profileMap[(*cstItr)->getKey()] = *cstItr;
            }
        }
    }

    TA_Base_Core::IProfile* ProfileConfigAccessImpl::getProfileByKey( unsigned long key) const
    {
        ProfileMap::const_iterator cstIter = m_profileMap.find(key);
        if (cstIter != m_profileMap.end())
        {
            return cstIter->second;
        }
        return NULL;
    }

    const ProfileMap& ProfileConfigAccessImpl::getAllProfile()
    {
        return m_profileMap;
    }

}