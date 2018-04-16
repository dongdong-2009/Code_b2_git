#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSTrainborneEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#define TRAINBORNE_EVENT_TRIGGER_LENGTH         17      //81
#define TRAINBORNE_DATA_PARAMETER_LENTH			11		//76
#define TRAINBORNE_ALARMEVENT_TRAINID_POS		 4
#define TRAINBORNE_DATA_START                    7

using namespace TA_IRS_App;

TVSSTrainborneEventReader::TVSSTrainborneEventReader(boost::shared_ptr<TVSSEvent> event)
  : TVSSEventReader(event,EVENT_TRAINBORNE_EVENT,TRAINBORNE_DATA_PARAMETER_LENTH)
{
//	initializeTrainborne();
}

TVSSTrainborneEventReader::~TVSSTrainborneEventReader(void)
{
}
unsigned char TVSSTrainborneEventReader::getTrainborneData(int dataIndex)
{
	//return  getParameterChar (TrainborneData[dataIndex]);
	return getParameterChar(dataIndex+TRAINBORNE_DATA_START);
	//unsigned char thisByte = getParameterChar( dataIndex/8 + TRAINBORNE_DATA_START);
	//bool result = ( thisByte>>(dataIndex%8)) & 0x1 ;
	//return result;

}

unsigned long TVSSTrainborneEventReader::getTrainID()
{
	return (unsigned long)getParameterChar(TRAINBORNE_ALARMEVENT_TRAINID_POS);
}

int TVSSTrainborneEventReader::getTrainborneDataLength()
{
//	return TrainborneData.size();
	return TRAINBORNE_EVENT_TRIGGER_LENGTH - TRAINBORNE_DATA_START;
}