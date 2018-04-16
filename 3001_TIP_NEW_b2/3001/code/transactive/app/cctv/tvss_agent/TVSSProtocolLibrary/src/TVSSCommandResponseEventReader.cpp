#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSCommandResponseEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#define COMMAND_RESPONSE_LENGTH                      8
#define COMMAND_RESPONSE_PARAMETER_LENTH             2
#define EVENT_STATUS_POSITION                        7  

using namespace TA_IRS_App;

TVSSCommandEventReader::TVSSCommandEventReader(boost::shared_ptr<TVSSEvent> event)
  : TVSSEventReader(event,EVENT_COMMAND_RESPONSE,COMMAND_RESPONSE_PARAMETER_LENTH)
{

}

TVSSCommandEventReader::~TVSSCommandEventReader(void)
{
}
unsigned char TVSSCommandEventReader::getCommandStatus () 
{
	return  getParameterChar ( EVENT_STATUS_POSITION ) ;
}