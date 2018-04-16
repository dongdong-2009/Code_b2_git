/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSMsgFactory.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef SDTSMSGFACTORY_H
#define SDTSMSGFACTORY_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App {

    // forward declaration
    class SDTSProtocolEvent;
    class TL_DataEvent;

	class SDTSMsgFactory  
	{
		public:

            /**
	          * createSDTSProtocolMsg
	          * Takes a raw SDTSProtocol class and examines the CM PDU Type field
              * and optionally the TL PDU Type to determine the base msg to construct
	          * 
	          * @exception Throws TransactiveException if there is a failure in 
              *            parsing the message
	          */
			static SDTSProtocolEvent* createSDTSProtocolMsg(SDTSProtocolEvent* x);

		private:
			// disabled methods
			SDTSMsgFactory();
			~SDTSMsgFactory();
			SDTSMsgFactory(const SDTSMsgFactory&);
			void operator=(const SDTSMsgFactory&);
	};
};

#endif
