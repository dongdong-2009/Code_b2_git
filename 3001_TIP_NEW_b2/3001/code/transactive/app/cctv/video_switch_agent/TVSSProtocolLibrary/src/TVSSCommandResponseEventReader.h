#pragma once

#include "TVSSEventReader.h"
#include "TVSSProtocol.h"


namespace TA_IRS_App
{

	class TVSSCommandEventReader : public TVSSEventReader
	{
	public:
		TVSSCommandEventReader(boost::shared_ptr<TVSSEvent> event);
		virtual ~TVSSCommandEventReader(void);
	public :
      unsigned char getCommandStatus () ;
	};
} ;
