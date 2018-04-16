#include "TVSSMethod.h"
#include "TVSSProtocol.h"

#define METHOD_HEADER_LENGTH                          5
#define METHOD_HEADER_LENGTH_START_POSITION           0
#define METHOD_HEADER_TRAIN_ID_START_POSITION         3
#define METHOD_HEADER_TRANSACTION_ID_START_POSITION   2
#define METHOD_HEADER_METHOD_LENGTH_POSITION          4
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
: ITcpMethod ( methodLength ,METHOD_HEADER_LENGTH , METHOD_HEADER_LENGTH_START_POSITION) 
{
	setByte ( METHOD_HEADER_LENGTH_FLAG_POSITION, METHED_START_FLAG ) ;
    setByte ( METHOD_HEADER_METHOD_LENGTH_POSITION, methodLength ) ;
	setByte ( METHOD_HEADER_METHOD_START_POSITION , methodType ) ;
	
}

TVSSMethod::~TVSSMethod(void)
{
}

void TVSSMethod::setTransactionId(unsigned char transactionId)
{
	setByte(METHOD_HEADER_TRANSACTION_ID_START_POSITION,transactionId);
}

void TVSSMethod::setTrainId ( unsigned char trainId ) 
{
	setByte ( METHOD_HEADER_TRAIN_ID_START_POSITION , trainId ) ;
}


unsigned long TVSSMethod::getMethodHeaderLength() const
{
	return METHOD_HEADER_LENGTH;
}


void TVSSMethod::setMethodHeaderLength(unsigned int messageSize)
{
	setLong(METHOD_HEADER_LENGTH_START_POSITION,messageSize);
}


unsigned long TVSSMethod::getShortStringLength() const
{
	return SHORTSTRING_LENGTH;
}
