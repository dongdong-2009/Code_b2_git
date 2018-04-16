#include "app/cctv/video_display_manager/src/PTZCommandListner.h"

namespace TA_IRS_App
{

	bool PTZCommandListener::onPTCommand( unsigned short ptDirection, bool isStopCommand )
	{
		return true;
	}

	bool PTZCommandListener::onZoomCommand( unsigned short in, bool isStopCommand )
	{
		return true;
	}

	bool PTZCommandListener::onPanCommand( unsigned short panDirection, bool isStopCommand )
	{
		return true;
	}

	bool PTZCommandListener::onTiltCommand( unsigned short tiltDirection, bool isStopCommand )
	{
		return true;
	}

	std::string PTZCommandListener::getErrorMessage()
	{
		return "";
	}

}