/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/RadioProtocol/MotorolaElite/SDTSProtocol/src/SDSResponse.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#ifndef __SDS_RESPONSE_H
#define __SDS_RESPONSE_H
	
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEvent.h"

namespace TA_IRS_App {

	class SDSResponse : public SDSEvent
	{
		public:

			SDSResponse();

			virtual ~SDSResponse();
			
			virtual int call();

		private:
            // disabled methods
            SDSResponse(const SDSResponse&);
            void operator=(const SDSResponse&);
    };

};
#endif

