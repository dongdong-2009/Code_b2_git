
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterResponse.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CM_REGISTERRESPONSE_H
#define CM_REGISTERRESPONSE_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterRequest.h"

namespace TA_IRS_App {

	class CM_RegisterResponse : public CM_RegisterRequest  
	{
		public:
			CM_RegisterResponse();
			virtual ~CM_RegisterResponse();

            bool parseData(const std::vector<unsigned char>& pData);
			int getRegistrationStatus();

		private:
			// disabled methods
			CM_RegisterResponse(const CM_RegisterResponse&);
			void operator=(const CM_RegisterResponse&);


        private:
			
			int m_registrationStatus;

        public:
            static const int CM_REGISTRATION_STATUS_INDEX;
            static const int CM_REGISTER_RESPONSE_LENGTH;
		
	};
};
#endif
