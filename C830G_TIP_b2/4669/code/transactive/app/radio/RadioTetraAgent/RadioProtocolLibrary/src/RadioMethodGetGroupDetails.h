/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodGetGroupDetails.h $
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
//  RadioMethodGetGroupDetails.h
//  Implementation of the Class RadioMethodGetGroupDetails
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodGetGroupDetails_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define RadioMethodGetGroupDetails_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethod.h"

#include <vector>

namespace TA_IRS_App
{

	class RadioMethodGetGroupDetails : public RadioMethod
	{
		public:
			RadioMethodGetGroupDetails(RadioSessionReference sessionRef,unsigned long subscriberRef);
			virtual ~RadioMethodGetGroupDetails();

            void setFields(std::vector<std::string> fields);

		private:
			RadioMethodGetGroupDetails(const RadioMethodGetGroupDetails& theRadioMethodGetGroupDetails);
            
            static const unsigned int GET_GROUP_DETAILS_BASE_LENGTH;
            static const unsigned int SUBSCRIBER_REFERENCE_PARAMETER_POSITION;
            static const unsigned int ROW_COUNT_PARAMETER_POSITION;
            static const unsigned int FIELDS_PARAMETER_START_POSITION;

            static const unsigned int MAX_ROWS_PER_EVENT;
	};
};
#endif // !defined(RadioMethodGetGroupDetails_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
