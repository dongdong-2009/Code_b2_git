
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon1.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CMCOMMON1_H
#define CMCOMMON1_H


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSProtocolEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	class CMCommon1 : public SDTSProtocolEvent  
	{
		public:
			CMCommon1();
			virtual ~CMCommon1();

			CMPDUType getCM_PDUType();
			void setCM_PDUType(CMPDUType pduType);

		private:
			// disabled methods
			CMCommon1(const CMCommon1&);
			void operator=(const CMCommon1&);

	    /* attributes */
		private:
			
			CMPDUType cmPDUType;

        public:
            static const int CM_PDUTYPE_INDEX;

	};
};
#endif
