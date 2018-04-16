#pragma once
#include <string>

namespace TA_IRS_App
{
class IPTZCommandListener
{
public:
	IPTZCommandListener(void) {}
	virtual ~IPTZCommandListener(void){}

	virtual bool onPTCommand ( unsigned short ptDirection, bool isStopCommand ) =0;

    virtual bool onZoomCommand( unsigned short in, bool isStopCommand ) =0;

    virtual bool onPanCommand ( unsigned short panDirection, bool isStopCommand )=0 ;

    virtual bool onTiltCommand ( unsigned short tiltDirection, bool isStopCommand)=0;

	virtual std::string getErrorMessage() = 0;
};
}
