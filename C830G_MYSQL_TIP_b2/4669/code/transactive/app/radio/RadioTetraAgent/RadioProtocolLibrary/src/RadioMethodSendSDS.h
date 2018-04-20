/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSendSDS.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodSendSDS.h
//  Implementation of the Class RadioMethodSendSDS
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodSendSDS_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define RadioMethodSendSDS_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "RadioMethodCallReference.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodSendSDS : public RadioMethodCallReference 
	{
		public:
			RadioMethodSendSDS(RadioSessionReference sessionRef, RadioCallReference callRef);
			virtual ~RadioMethodSendSDS();

		public:
			void setDestination(std::string destination);
			void setMessage(std::vector<unsigned char> data);
			void setPreCode(short precode);

		private:
			RadioMethodSendSDS(const RadioMethodSendSDS& theRadioMethodSendSDS);
	};
};
#endif // !defined(RadioMethodSendSDS_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
