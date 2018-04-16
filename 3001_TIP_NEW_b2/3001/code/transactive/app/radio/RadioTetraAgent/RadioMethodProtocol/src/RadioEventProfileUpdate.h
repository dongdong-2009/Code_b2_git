
#ifndef RADIO_EVENT_PROFILE_UPDATE_H
#define RADIO_EVENT_PROFILE_UPDATE_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

namespace TA_IRS_App
{
	class RadioEventProfileUpdate : public RadioEventReader
	{
	public:
		RadioEventProfileUpdate(boost::shared_ptr<RadioEvent> event);
		virtual ~RadioEventProfileUpdate();

	public:

		int getNumDataSet();

		/**
		 * Return data set (s)                                                         
		 **/
		std::vector<std::pair<std::string, short> > getDataSet();
	private:
		int numDataSet;
		const static unsigned short STATUS_TYPE_LENGTH;
		const static unsigned short EVENT_HEADER_PROFILE_STATUS_NUM_POSITION;
		const static unsigned short EVENT_HEADER_PROFILE_STATUS_INTERVAL;
	};
};
#endif // end of RADIO_EVENT_PROFILE_REQUEST_H