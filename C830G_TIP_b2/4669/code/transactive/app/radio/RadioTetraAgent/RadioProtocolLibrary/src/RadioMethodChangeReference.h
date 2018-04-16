/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodChangeReference.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Change Reference method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodChangeReference.h
//  Implementation of the Class RadioMethodChangeReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodChangeReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define RadioMethodChangeReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodCallReference.h"


#include <vector>

namespace TA_IRS_App
{
    struct CallForwardingDetails; // Forward declaration

	class RadioMethodChangeReference : public RadioMethodCallReference 
	{
		public:
            enum ESearchType
            {
                FULL,
                NORMAL
            };

			RadioMethodChangeReference(RadioSessionReference sessionRef, RadioCallReference callRef);
			virtual ~RadioMethodChangeReference();

            void setForwardingDetails(const CallForwardingDetails& details);

            void setPatchDetails(const std::string& controllingITSI,
								 const std::vector<std::string>& patchList,
                                 const std::string& patchName);

			void setGroupMembers(const std::string& memberList);

            void setSearchType(const ESearchType searchType);
	
			void setAudioVolume(int leftVolume,int rightVolume);

		private:
			RadioMethodChangeReference(const RadioMethodChangeReference& theRadioMethodChangeReference);
            
  /*          static const unsigned int CHANGE_REFERENCE_BASE_LENGTH;
            static const unsigned int DETAILS_PARAMETER_START_POSITION;

            // String keys
            static const std::string ENABLE_FORWARDING;
            static const std::string DISABLE_FORWARDING;
            static const std::string FORWARD_UNCONDITIONAL;
            static const std::string FORWARD_NOT_REACHABLE;
            static const std::string TYPE;
            static const std::string PATCH_ID;
            static const std::string OWNER_DISPATCHER;
            static const std::string PATCH_HANG_TIME;
            static const std::string MEMBERS;
            static const std::string SEARCH_MODE;

            // specific key values
            static const std::string PATCH_TYPE;
            static const std::string FULL_SEARCH;
            static const std::string NORMAL_SEARCH;

            // Constant values that will be replaced
            static const std::string PATCH_HANG_TIME_VALUE;

            // Constant protocol values (defined as a constant so any changes to the protocol down the line don't 
            // hurt us)
            static const std::string KEY_SEPARATOR;
            static const std::string VECTOR_SEPARATOR;
			*/
	};
};
#endif // !defined(RadioMethodChangeReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
