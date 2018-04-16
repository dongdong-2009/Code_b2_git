/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_DataEvent.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_DataEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


using namespace TA_Base_Core;

namespace TA_IRS_App {
    
	const int TL_DataEvent::TL_DATA_HEADER_LENGTH = 6;

	const int TL_DataEvent::TL_DATA_FLAG_INDEX = 16;
	const int TL_DataEvent::TL_DATA_VALIDITY_INDEX = 17;
	const int TL_DataEvent::TL_DATA_FORWARD_INDEX = 18;
	const int TL_DataEvent::TL_DATA_USER_INDEX = 21;

	const int TL_DataEvent::TL_BROADCAST_FORWARD_INDEX = 22;
	
	const int TL_DataEvent::TL_BROADCAST_USER_INDEX = 25;
			
	TL_DataEvent::TL_DataEvent() : m_pTrainborneSDS(NULL),
								   m_pTxtMsg(NULL),
								   receivedConfirmationRequested(false),
								   consumedConfirmationRequested(false),
								   storageAllowed(false),
								   validityReport(0),
								   forwardAddress(0),
                                   TLCommon(TLPDUTYPE_DATA)
	{
		FUNCTION_ENTRY("constructor");

		FUNCTION_EXIT;
	}

	TL_DataEvent::~TL_DataEvent()
	{
		FUNCTION_ENTRY("destructor");

		if ( m_pTrainborneSDS != NULL )
		{
			delete m_pTrainborneSDS;
			m_pTrainborneSDS = NULL;
		}

		if ( m_pTxtMsg != NULL )
		{
			delete m_pTxtMsg;
			m_pTxtMsg = NULL;
		}

		FUNCTION_EXIT;
	}

	bool TL_DataEvent::getRECflag()
	{
		return receivedConfirmationRequested;
	}

	bool TL_DataEvent::getCONflag()
	{
		return consumedConfirmationRequested;
	}

	bool TL_DataEvent::getSTOflag()
	{
		return storageAllowed;
	}

	void TL_DataEvent::setRECflag( bool flag )
	{
		receivedConfirmationRequested = flag;
	}

	void TL_DataEvent::setCONflag( bool flag )
	{
		consumedConfirmationRequested = flag;
	}

	void TL_DataEvent::setSTOflag( bool flag )
	{
		storageAllowed = flag;
	}

	unsigned char TL_DataEvent::getValidityPeriod()
	{
		return validityReport;
	}

	unsigned long TL_DataEvent::getForwardAddress()
	{
		return forwardAddress;
	}

	unsigned short TL_DataEvent::getTLDataLength()
	{
		CMCommon3* pCMHeader = getCMHeader();
        if (pCMHeader == NULL) return 0;

		CMProtocolIdentifier pi = pCMHeader->getProtocolId();

        int msgLength = 0;

		switch(pi)
		{
			case CMPROTOCOLIDENTIFIER_TEXTMSG:
                msgLength = m_pTxtMsg->getText().length();
				break;

			case CMPROTOCOLIDENTIFIER_TRAINBORNEMSG:
                msgLength = m_pTrainborneSDS->getSDSMessage().size();
				break;	

            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Invalid pi %d", pi);
                return 0;
		}

        // 1 byte prepended for TextCodingScheme
		// wong.peter, DTL omit TextCodingScheme
		return (msgLength + TL_DATA_HEADER_LENGTH) * 8;
	}


	void TL_DataEvent::setValidityPeriod( unsigned char vReport )
	{
		validityReport = vReport;
	}

	void TL_DataEvent::setForwardAddress( unsigned long forwardAddr)
	{
		forwardAddress = forwardAddr;
	}

	void TL_DataEvent::setTrainborneSDSData(msgDataFormat_TrainborneSDS* pTrainborneSDS)
	{
		m_pTrainborneSDS = pTrainborneSDS;
	}

	void TL_DataEvent::setTextMessage(msgDataFormat_TextMsg* pTxtMsg)
	{
		m_pTxtMsg = pTxtMsg;
	}

    msgDataFormat_TrainborneSDS* TL_DataEvent::getTrainborneSDSData()
    {
        return m_pTrainborneSDS;
    }

    msgDataFormat_TextMsg* TL_DataEvent::getTextMessage()
    {
        return m_pTxtMsg;
    }


	void TL_DataEvent::buildMsg()
	{
		FUNCTION_ENTRY("buildMsg");

		CMCommon3* pCMHeader = getCMHeader();

		if ( pCMHeader != NULL )
		{
			std::vector<unsigned char> tlData = pCMHeader->getRawMessage();

			// set CM TL length
			std::vector<unsigned char> tempData = SDTSHelper::encode_u16( getTLDataLength() );
			tlData.insert( tlData.end(), tempData.begin(), tempData.end() );
			
			// PDU type
			tempData = SDTSHelper::encodeTL_PDUType( getTL_PDUType() );
			tlData.insert( tlData.end(), tempData.begin(), tempData.end() );

			// TL flags
			tempData = SDTSHelper::encodeTL_Flag(receivedConfirmationRequested,consumedConfirmationRequested,storageAllowed);
			tlData.insert( tlData.end(), tempData.begin(), tempData.end() );
				
			// Validity period
			tlData.push_back( validityReport );

			// Forward Address
			TA_ASSERT(forwardAddress <= 0xFFFFFF , "forwardAddress must be 3 bytes");
			tempData = SDTSHelper::encode_u24(forwardAddress);
			tlData.insert( tlData.end(), tempData.begin(), tempData.end() );

			CMProtocolIdentifier pi = pCMHeader->getProtocolId();

			switch(pi)
			{
				case CMPROTOCOLIDENTIFIER_TEXTMSG:
					{
						TA_ASSERT(m_pTxtMsg != NULL, "msgDataFormat_TextMsg is NULL");
						std::string text = m_pTxtMsg->getText();
						// peter.wong, DTL-480, update format
						//tlData.push_back(static_cast<unsigned char>(m_pTxtMsg->getTextCodingScheme()));
						for (unsigned int i=0 ;i < text.size(); i++)
						{
							tlData.push_back(text[i]);
						}
					}
					break;

				case CMPROTOCOLIDENTIFIER_TRAINBORNEMSG:
					
					TA_ASSERT(m_pTrainborneSDS != NULL, "msgDataFormat_TrainborneSDS is NULL");
					// peter.wong, DTL-480, update format
					//tlData.push_back(TEXTCODING_ISO_88591);
					tempData = m_pTrainborneSDS->getSDSMessage();
					tlData.insert( tlData.end(), tempData.begin(), tempData.end() );
					break;	

                default:
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Invalid pi %d", pi);
                    return;
			}

			// set Adaptation length, it's already preallocated by the CMCommon3
			unsigned int nAdaptation = tlData.size() - 2;
			tempData = SDTSHelper::encode_u16(nAdaptation);
            tlData[0] = tempData[0];
            tlData[1] = tempData[1];

			setRawMessage(tlData);
		}		
		FUNCTION_EXIT;
	}

	
	bool TL_DataEvent::parseData( const std::vector<unsigned char>& rawData)
	{
		FUNCTION_ENTRY("parseData");

		int pduTypeIndex, nIndex;
		bool result = true;

		CMCommon3* pCMHeader = getCMHeader();
		CMPDUType cmPDUType = pCMHeader->getCM_PDUType();

		if ( cmPDUType == CMPDUTYPE_BROADCAST ) 
		{
			pduTypeIndex = TL_BROADCAST_PDU_TYPE_INDEX;
			nIndex = 4;
		}
		else if ( cmPDUType == CMPDUTYPE_DATA_REQ )
		{
			pduTypeIndex = TL_DATA_PDU_TYPE_INDEX;
			nIndex = 0;
		}
		else
		{
			TA_THROW(TA_Base_Core::TransactiveException("Invalid CM PDU Type"));
			return false;
		}

		unsigned short tlLength = pCMHeader->getTLlength();
		unsigned short nCmTLlength = tlLength / 8;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "CMHeader TL Length ---> [%d]",rawData.size()- pduTypeIndex);
		if (tlLength % 8 != 0) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid CM TL length: [%d], not multiplier of 8",tlLength);
			return false;
		}
		if ( nCmTLlength != rawData.size()- pduTypeIndex )
		{
			TA_THROW(TA_Base_Core::TransactiveException(std::string("Invalid CM TL length[%d]",nCmTLlength)));
			return false;
		}

		
		std::vector<unsigned char> vPDUtype(1, rawData[pduTypeIndex]);
		TLPDUType pduType = SDTSHelper::decodeTL_PDUType(vPDUtype);
		if (pduType != TLPDUTYPE_DATA)
		{
			TA_THROW(TA_Base_Core::TransactiveException("Invalid TL PDU Type"));
			return false;
		}		        

		// TL Flags
		setRECflag(SDTSHelper::decodeTL_REC(rawData[TL_DATA_FLAG_INDEX+nIndex]));
		setCONflag(SDTSHelper::decodeTL_CON(rawData[TL_DATA_FLAG_INDEX+nIndex]));
		setSTOflag(SDTSHelper::decodeTL_STO(rawData[TL_DATA_FLAG_INDEX+nIndex]));

		// Validity period
		setValidityPeriod( rawData[TL_DATA_VALIDITY_INDEX+nIndex]);
		
		// Forward Address
		int forwardAddress = 0;
		if ( cmPDUType == CMPDUTYPE_BROADCAST ) 
		{
			std::vector<unsigned char> vForwardAddr(rawData.begin()+TL_BROADCAST_FORWARD_INDEX, rawData.begin()+TL_BROADCAST_FORWARD_INDEX+3);
			forwardAddress = SDTSHelper::decode_u32(vForwardAddr);
			setForwardAddress(forwardAddress);

			nIndex = TL_BROADCAST_USER_INDEX;
		}
		else if ( cmPDUType == CMPDUTYPE_DATA_REQ )
		{
			std::vector<unsigned char> vForwardAddr(rawData.begin()+TL_DATA_FORWARD_INDEX, rawData.begin()+TL_DATA_FORWARD_INDEX+3);
			forwardAddress = SDTSHelper::decode_u32(vForwardAddr);
			setForwardAddress(forwardAddress);

			nIndex = TL_DATA_USER_INDEX;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Forward Address = %d",forwardAddress);
                
		//unsigned short txtCodingScheme = rawData[nIndex++];
		// User Data
		CMProtocolIdentifier pi = pCMHeader->getProtocolId();
		switch(pi)
		{
			case CMPROTOCOLIDENTIFIER_TEXTMSG:    
			{
				 m_pTxtMsg = new msgDataFormat_TextMsg();
				
				 // peter.wong, DTL-480, update format
				 //m_pTxtMsg->setTextCodingScheme(txtCodingScheme);
				 
				 std::vector<unsigned char>::const_iterator iter;

				 std::string text;
				 for ( iter=rawData.begin()+nIndex; iter != rawData.end(); iter++ )
				 {
					 text += *iter;
				 }

				 m_pTxtMsg->setText(text);
			}		 
			break;
							 
			case CMPROTOCOLIDENTIFIER_TRAINBORNEMSG:	
			{	
				m_pTrainborneSDS = new msgDataFormat_TrainborneSDS();
				
				std::vector<unsigned char>  sdsMsg(rawData.begin()+nIndex,rawData.end());
				m_pTrainborneSDS->setSDSMessage(sdsMsg);
			}
			break;

            default:
                TA_THROW(TA_Base_Core::TransactiveException(std::string("Invalid pi %d", pi)));
				result = false;
                break;
		}
		
		FUNCTION_EXIT;
		return result;
	}

}
