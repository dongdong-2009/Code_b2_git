#include "TVSSCommandResponseEventReader.h"
#include "TVSSProtocol.h"

#define COMMAND_RESPONSE_LENGTH                      7
#define EVENT_STATUS_POSITION                        6  

using namespace TA_IRS_App;

TVSSCommandEventReader::TVSSCommandEventReader(boost::shared_ptr<TVSSEvent> event)
  : TVSSEventReader(event,EVENT_COMMAND_RESPONSE,COMMAND_RESPONSE_LENGTH)
{

}

TVSSCommandEventReader::~TVSSCommandEventReader(void)
{
}
unsigned char TVSSCommandEventReader::getCommandStatus () 
{
	return  getParameterChar ( EVENT_STATUS_POSITION ) ;
}