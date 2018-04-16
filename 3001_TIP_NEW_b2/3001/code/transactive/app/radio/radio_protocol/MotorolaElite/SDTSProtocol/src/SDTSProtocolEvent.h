/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSProtocolEvent.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef SDTSPROTOCOLEVENT_H
#define SDTSPROTOCOLEVENT_H

#include <vector>

namespace TA_IRS_App {

	class SDTSProtocolEvent  
	{
		public:
			SDTSProtocolEvent();
			virtual ~SDTSProtocolEvent();

			unsigned int getAdaptationLength();
			void setAdaptationLength(unsigned int nAdaption);

			void setRawMessage(const std::vector<unsigned char>&  rawMsg);
			std::vector<unsigned char> getRawMessage(); 
			

		protected:
			virtual void buildMsg();

			std::vector<unsigned char>  rawMsg;

		private:
			// disabled methods
			SDTSProtocolEvent(const SDTSProtocolEvent&);
			void operator=(const SDTSProtocolEvent&);

			unsigned int                adaptionLength;

			bool  rawMsgInitialised;
            
        public:
            static const int ADAPTATION_LENGTH;
	};
};
#endif
