
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterRequest.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CM_REGISTERREQUEST_H
#define CM_REGISTERREQUEST_H


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon1.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	class CM_RegisterRequest : public CMCommon1  
	{
		public:
			
			CM_RegisterRequest();
			CM_RegisterRequest(unsigned long passNumber, unsigned long clientSSI, CMRegistrationType regType);
			virtual ~CM_RegisterRequest();

			unsigned long getPassNumber();
			unsigned long getClientSSI();
			CMRegistrationType getRegistrationType();

			void setPassNumber(unsigned long passNum);
			void setClientSSI(unsigned long clientSSI);
			void setRegistrationType(CMRegistrationType regType);


		protected:

			virtual void buildMsg();

		private:
			// disabled methods
			CM_RegisterRequest(const CM_RegisterRequest&);
			void operator=(const CM_RegisterRequest&);


		// attributes
		private:

			CMRegistrationType m_registrationType;

			unsigned long m_passNumber;
			unsigned long m_clientSSI;

        public:
            static const int CM_PASS_NUMBER_INDEX;
            static const int CM_CLIENT_SSI_INDEX;
            static const int CM_REGISTRATION_TYPE_INDEX;
	};
};

#endif
