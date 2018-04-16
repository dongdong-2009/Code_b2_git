/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_DataEvent.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef TL_DATAEVENT_H
#define TL_DATAEVENT_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TextMsg.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TrainborneSDS.h"


namespace TA_IRS_App {

	class TL_DataEvent : public TLCommon  
	{
		public:
			TL_DataEvent();
			virtual ~TL_DataEvent();
		
			bool getRECflag();
			bool getCONflag();
			bool getSTOflag();

			void setRECflag( bool flag );
			void setCONflag( bool flag );
			void setSTOflag( bool flag );

			unsigned char getValidityPeriod();
			unsigned long getForwardAddress();
			unsigned short getTLDataLength();

			void setValidityPeriod( unsigned char validityPeriod );
			void setForwardAddress( unsigned long forwardAddr);

			void setTrainborneSDSData(msgDataFormat_TrainborneSDS* pTrainborneSDS);
			void setTextMessage(msgDataFormat_TextMsg* pTxtMsg);

            msgDataFormat_TrainborneSDS* getTrainborneSDSData();
            msgDataFormat_TextMsg* getTextMessage();

			bool parseData( const std::vector<unsigned char>& rawData);

		protected:
			
			virtual void buildMsg();


		private:
			// disabled methods
			TL_DataEvent(const TL_DataEvent&);
			void operator=(const TL_DataEvent&);

			std::vector<unsigned char> buildTextMsg();
			std::vector<unsigned char> buildTrainborneSDS();

			
		/* attributes */
		private:

			bool receivedConfirmationRequested; 
			bool consumedConfirmationRequested;
			bool storageAllowed;
			unsigned char validityReport; 
			unsigned long forwardAddress; 

			// user data
			msgDataFormat_TrainborneSDS* m_pTrainborneSDS;
			msgDataFormat_TextMsg* m_pTxtMsg;
			

		public:

            static const int TL_DATA_HEADER_LENGTH;
			static const int TL_DATA_FLAG_INDEX;
			static const int TL_DATA_VALIDITY_INDEX;
			static const int TL_DATA_FORWARD_INDEX;
			static const int TL_DATA_USER_INDEX;

			static const int TL_BROADCAST_FORWARD_INDEX;
			static const int TL_BROADCAST_USER_INDEX;


	};
};
#endif
