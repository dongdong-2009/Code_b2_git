#include "TVSSTrainborneEventReader.h"
#include "TVSSProtocol.h"

#define TRAINBORNE_EVENT_TRIGGER_LENGTH         81
#define TRAINBORNE_DATA_LENTH                   75
#define TRAINBORNE_DATA_START                    6

using namespace TA_IRS_App;

TVSSTrainborneEventReader::TVSSTrainborneEventReader(boost::shared_ptr<TVSSEvent> event)
  : TVSSEventReader(event,EVENT_TRAINBORNE_EVENT,TRAINBORNE_EVENT_TRIGGER_LENGTH)
{
	initializeTrainborneMap();
}

TVSSTrainborneEventReader::~TVSSTrainborneEventReader(void)
{
}
unsigned char TVSSTrainborneEventReader::getTrainborneData(int dataIndex)
{
	return  getParameterChar ( TrainborneData[dataIndex] );
}

void TVSSTrainborneEventReader::initializeTrainborneMap(void)
{
	int dataStart = TRAINBORNE_DATA_START;
	for(int x = 1; x <=  TRAINBORNE_DATA_LENTH; x++)
		TrainborneData[x] = dataStart++;
}