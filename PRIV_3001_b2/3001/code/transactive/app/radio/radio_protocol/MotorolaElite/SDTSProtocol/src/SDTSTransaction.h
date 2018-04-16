
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransaction.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef __SDTS_Transaction_H
#define __SDTS_Transaction_H	

#include "ace/OS.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

	struct SDTSTransaction
	{
		unsigned long src;
		unsigned long dst;
		CMProtocolIdentifier pi;

		unsigned char messageRef;

		ACE_Time_Value timestamp;
	};
 
}; 


#endif //__SDTS_Transaction_H