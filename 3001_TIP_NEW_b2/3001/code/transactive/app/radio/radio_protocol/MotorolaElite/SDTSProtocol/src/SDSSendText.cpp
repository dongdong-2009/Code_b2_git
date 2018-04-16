/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSSendText.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is responsible for sending text messages
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSSendText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_DataEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Broadcast.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TextMsg.h"


namespace TA_IRS_App {

    SDSSendText::SDSSendText(SDTSTransactionManager& transactionManager, 
                             SDSQueueManager& queueManager,
                             unsigned int srcSsi, unsigned int destSsi, CMProtocolIdentifier pi) : 
                    m_transactionManager(transactionManager),
                    m_queueManager(queueManager),
                    m_srcSsi(srcSsi), m_destSsi(destSsi), m_pi(pi),
                    //m_textCodingScheme(TEXTCODING_ISO_88591),
					m_isBroadcast(false)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}

	SDSSendText::~SDSSendText()
	{
		FUNCTION_ENTRY( "Destructor" );

		FUNCTION_EXIT;
	}

	int SDSSendText::call()
	{
		FUNCTION_ENTRY( "call" );

		try
		{
			if (m_isBroadcast)
			{
				createBroadcastMessage();
				return 0;
			}

			// begin transaction
			unsigned char msgRef = m_transactionManager.beginTransaction(
                                        m_srcSsi, m_destSsi, m_pi);

            TL_DataEvent msg;
            
            CM_Data* cmData = new CM_Data();

            cmData->setSourceSSI(m_srcSsi);
            cmData->setDestSSI(m_destSsi);
            cmData->setMessageRef(msgRef);
            cmData->setProtocolId(m_pi);

            if (m_pi == CMPROTOCOLIDENTIFIER_TRAINBORNEMSG)
            {
                msgDataFormat_TrainborneSDS* sdsMsg = new msgDataFormat_TrainborneSDS();
                sdsMsg->setSDSMessage(m_sdsMessage);
                msg.setTrainborneSDSData(sdsMsg);
            }
            else
            {
                msgDataFormat_TextMsg* textMsg = new msgDataFormat_TextMsg();

                textMsg->setText(m_text);
                //textMsg->setTextCodingScheme(m_textCodingScheme);
                msg.setTextMessage(textMsg);
            }

            msg.setCMHeader(cmData);
			
			// send message 
			m_queueManager.sendMessage(msg.getRawMessage());
		}
		catch(TA_Base_Core::TransactiveException e)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			return -1;
		}

		return 0;
		FUNCTION_EXIT;
	}

    void SDSSendText::setSDSMessage(const std::vector<unsigned char> msg)
    {
        m_sdsMessage = msg;
    }

    void SDSSendText::setText(const std::string& text)
    {
        m_text = text;
    }


    //void SDSSendText::setTextCodingScheme(TextCodingScheme scheme)
    //{
    //    m_textCodingScheme = scheme;
    //}

	void SDSSendText::setBroadcastMode(bool isBroadcast)
	{
		m_isBroadcast = isBroadcast;
	}

	void SDSSendText::createBroadcastMessage()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "createBroadcastMessage");
		// begin transaction
		unsigned char msgRef = m_transactionManager.beginTransaction(
                                    m_srcSsi, m_destSsi, m_pi);

        TL_DataEvent msg;
        
        CM_Broadcast* cmBroadcast = new CM_Broadcast();

        cmBroadcast->setSourceSSI(m_srcSsi);
        cmBroadcast->setDestSSI(m_destSsi);
        cmBroadcast->setMessageRef(msgRef);
        cmBroadcast->setProtocolId(m_pi);

        msgDataFormat_TextMsg* textMsg = new msgDataFormat_TextMsg();

        textMsg->setText(m_text);
        //textMsg->setTextCodingScheme(m_textCodingScheme);
        msg.setTextMessage(textMsg);

        msg.setCMHeader(cmBroadcast);
		
		// send message 
		m_queueManager.sendMessage(msg.getRawMessage());
	}
}
