/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSSendText.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is responsible for sending text messages
  */



#ifndef __SDS_SendText_H
#define __SDS_SendText_H

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSFunction.h"	
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"	

namespace TA_IRS_App {
    class SDTSTransactionManager;
    class SDSQueueManager;

	class SDSSendText : public SDSFunction
	{
		public:

			SDSSendText(SDTSTransactionManager& transactionManager, 
                        SDSQueueManager& queueManager, 
                        unsigned int srcSsi, unsigned int destSsi, CMProtocolIdentifier pi);

			virtual ~SDSSendText();

			virtual int call();
            
            void setSDSMessage(const std::vector<unsigned char> msg);

            void setText(const std::string& text);
			// peter.wong, DTL-480, update format
			//void setTextCodingScheme(TextCodingScheme scheme); 

			void setBroadcastMode(bool isBroadcast);
			void createBroadcastMessage();

        private:
            // disabled methods
            SDSSendText(const SDSSendText&);
            void operator=(const SDSSendText&);
            
		private:
            unsigned int m_srcSsi;
            unsigned int m_destSsi;
            CMProtocolIdentifier m_pi;

            std::vector<unsigned char> m_sdsMessage;
            
            std::string m_text;
            //TextCodingScheme m_textCodingScheme;

            SDTSTransactionManager& m_transactionManager;
            SDSQueueManager& m_queueManager;

			bool m_isBroadcast;

	};


};


#endif //__SDS_SendText_H
