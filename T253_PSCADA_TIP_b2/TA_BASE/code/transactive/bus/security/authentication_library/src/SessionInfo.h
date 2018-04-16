/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/authentication_library/src/SessionInfo.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 *
 * This struct contains all the relevant information about a session.
 */

#if !defined(SESSION_INFO_H)
#define SESSION_INFO_H

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <string>
#include <vector>

namespace TA_Base_Bus
{
	namespace Authentication
	{
        static const unsigned int BASE_OPERATOR = 0;
		static const unsigned int BASE_PROFILE =0;
	}

    struct SessionInfo
    {
        std::string SessionId;
        unsigned long UserId; // UserId is essentially redundant now because of the OperatorId sequence,
                              // however it is kept in place to reduce the number of modifications required
                              // for TD11122.
        std::vector<unsigned long> OperatorId;
        std::vector<unsigned long> ProfileId;
        unsigned long LocationId;
        unsigned long WorkstationId;
        //the Change//
		bool isDisplayOnly;
    };
} // end namespace TA_Base_App

#endif // !defined(SESSION_INFO_H)

