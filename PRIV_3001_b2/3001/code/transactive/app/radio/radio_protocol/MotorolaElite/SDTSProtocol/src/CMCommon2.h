
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon2.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CMCOMMON2_H
#define CMCOMMON2_H


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon1.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	class CMCommon2 : public CMCommon1  
	{
		public:
			CMCommon2();
			virtual ~CMCommon2();

			// get methods
			unsigned long getSourceSSI();
			unsigned long getDestSSI();
			unsigned char getMessageRef();
			CMProtocolIdentifier getProtocolId();

			// set methods
			void setSourceSSI( unsigned long srcSSI );
			void setDestSSI(unsigned long dstSSI );
			void setMessageRef( unsigned char msgRef );
			void setProtocolId( CMProtocolIdentifier pi);

		private:
			// disabled methods
			CMCommon2(const CMCommon2&);
			void operator=(const CMCommon2&);

			
		/* attributes */	
		private:

			CMProtocolIdentifier cmProtocolIdentifier;
			// TODO
			unsigned long sourceSSI;
			unsigned long destSSI;
			unsigned char cmMsgRef;

        public:
            static const int CM_SOURCE_SSI_INDEX;
            static const int CM_DESTINATION_SSI_INDEX;
            static const int CM_PI_INDEX;
            static const int CM_MESSAGE_REF_INDEX;
	};
};
#endif
