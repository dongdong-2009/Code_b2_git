
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSFunction.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef __SDS_Function_H
#define __SDS_Function_H

#include "ace/Method_Request.h"


namespace TA_IRS_App {

	class SDSFunction: public ACE_Method_Request
	{
		public:
			SDSFunction();
			virtual ~SDSFunction();
			
			virtual int call() = 0;

        private:
            // disabled methods
            SDSFunction(const SDSFunction&);
            void operator=(const SDSFunction&);
	};

};

	
#endif //__SDS_Function_H

