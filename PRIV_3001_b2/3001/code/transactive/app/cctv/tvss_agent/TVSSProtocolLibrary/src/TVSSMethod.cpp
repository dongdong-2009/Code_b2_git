#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/crc16.h"

#define METHOD_HEADER_LENGTH                          6
#define METHOD_HEADER_LENGTH_START_POSITION           0
#define METHOD_HEADER_TRAIN_ID_START_POSITION         4
#define METHOD_HEADER_TRANSACTION_ID_START_POSITION   2
#define METHOD_HEADER_METHOD_LENGTH_POSITION          5
#define METHOD_HEADER_METHOD_START_POSITION           1
#define METHOD_HEADER_LENGTH_FLAG_POSITION            0

#define METHED_START_FLAG                             0x07



using namespace TA_IRS_App;

/*
* Constructor
*
* Creates a new radio method (message).
*
* @param    methodLength Length of parameter block for this message. 
* 
*/
TVSSMethod::TVSSMethod(unsigned int methodLength, unsigned char methodType )     
: ITcpMethod ( methodLength ,METHOD_HEADER_LENGTH , METHOD_HEADER_METHOD_LENGTH_POSITION )//METHOD_HEADER_LENGTH_START_POSITION) 
{
	setByte ( METHOD_HEADER_LENGTH_FLAG_POSITION, METHED_START_FLAG ) ;
  //  setByte ( METHOD_HEADER_METHOD_LENGTH_POSITION, methodLength ) ;
	setByte ( METHOD_HEADER_METHOD_START_POSITION , methodType ) ;
	
}

TVSSMethod::~TVSSMethod(void)
{
}

void TVSSMethod::setTransactionId(unsigned short transactionId)
 {
	setShort(METHOD_HEADER_TRANSACTION_ID_START_POSITION,transactionId);
}

void TVSSMethod::setTrainId ( unsigned char trainId ) 
{
	setByte ( METHOD_HEADER_TRAIN_ID_START_POSITION , trainId ) ;
}


unsigned long TVSSMethod::getMethodHeaderLength() const
{
	return METHOD_HEADER_LENGTH;
}


unsigned long TVSSMethod::getShortStringLength() const
{
	return SHORTSTRING_LENGTH;
}

void TVSSMethod::appendCRC()
{
	unsigned short crc = 0;
	unsigned long oriMethodLen = getMessageDataSize();
	setMessageSize(oriMethodLen + 2);
	crc = crc16(const_cast<unsigned char*>(getMessageData()), oriMethodLen);
	setShort(oriMethodLen, crc);
}