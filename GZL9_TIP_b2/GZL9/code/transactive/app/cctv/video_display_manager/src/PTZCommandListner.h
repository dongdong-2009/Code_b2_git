#pragma once

#include "app/cctv/video_display_manager/src/IPTZCommandListener.h"

namespace TA_IRS_App
{
class PTZCommandListener : public IPTZCommandListener
{
public:
	PTZCommandListener(void) {}
	virtual ~PTZCommandListener(void){}

	virtual bool onPTCommand ( unsigned short ptDirection, bool isStopCommand );

    virtual bool onZoomCommand( unsigned short in, bool isStopCommand );

    virtual bool onPanCommand ( unsigned short panDirection, bool isStopCommand );

    virtual bool onTiltCommand ( unsigned short tiltDirection, bool isStopCommand);

	virtual std::string getErrorMessage();
};
}