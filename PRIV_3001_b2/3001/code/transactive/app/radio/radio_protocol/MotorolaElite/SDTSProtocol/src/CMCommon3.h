
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CMCOMMON3_H
#define CMCOMMON3_H


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon2.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	class CMCommon3 : public CMCommon2  
	{
		public:
			CMCommon3();
			virtual ~CMCommon3();


			CMSDSType getSDSType();
			CMServiceSelection getServiceSelection();
			unsigned short getTLlength();

			void setSDSType(CMSDSType sdsType);
			void setServiceSelection(CMServiceSelection srvSelection);
			void setTLlength(unsigned short length);

		private:
			// disabled methods
			CMCommon3(const CMCommon3&);
			void operator=(const CMCommon3&);


		/* attributes */
		private:
			
			CMSDSType cmSDSType;
			CMServiceSelection cmServiceSelection;
			unsigned short tlBitLength;
			
		public:

            static const int CM_FLAG_INDEX;

	};
};

#endif
