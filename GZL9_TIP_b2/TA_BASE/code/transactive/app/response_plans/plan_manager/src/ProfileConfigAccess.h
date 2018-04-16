//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ProfileConfigAccess.h $
// @author:  Huang Jian
// @version: $Revision: #2 $
//
//
// This is a utility class. It provides methods for determining a data point's type and,
// where applicable, configured states.

#ifndef PROFILE_CONFIG_ACCESS_H
#define PROFILE_CONFIG_ACCESS_H

#include <map>
#include <vector>
#include "ace/Singleton.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Core
{
    class IProfile;
}

namespace TA_Base_App
{
    typedef std::map<unsigned long, TA_Base_Core::IProfile*> ProfileMap;
    class ProfileConfigAccessImpl
    {
    public:
        friend class ACE_Singleton<ProfileConfigAccessImpl, ACE_Recursive_Thread_Mutex>;
        TA_Base_Core::IProfile* getProfileByKey(unsigned long key) const;
        const ProfileMap& getAllProfile();
    protected:
    private:
        ProfileConfigAccessImpl();
        ~ProfileConfigAccessImpl();
        ProfileConfigAccessImpl( const ProfileConfigAccessImpl& );
        ProfileConfigAccessImpl& operator=( const ProfileConfigAccessImpl& );

        ProfileMap m_profileMap;
    };
    typedef ACE_Singleton<ProfileConfigAccessImpl, ACE_Recursive_Thread_Mutex> ProfileConfigAccess;
}

#endif