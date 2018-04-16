/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRecvText.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef __SDS_RecvText_H
#define __SDS_RecvText_H
	
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEvent.h"

namespace TA_IRS_App {

	class SDSRecvText : public SDSEvent
	{
		public:

			SDSRecvText();

			virtual ~SDSRecvText();
			
			virtual int call();

		private:
            // disabled methods
            SDSRecvText(const SDSRecvText&);
            void operator=(const SDSRecvText&);
    };

};


#endif //__SDS_RecvText_H


