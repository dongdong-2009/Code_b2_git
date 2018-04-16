
#ifndef _REQUEST_OCC_CALL_H_
#define _REQUEST_OCC_CALL_H_
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimsCommand.h"

namespace TA_IRS_App
{
	class RequestOccCall : public TimsCommand
	{
	public:
		RequestOccCall();
		~RequestOccCall();

		//Interface for TimsCommand
		int getCommandType() const;
		std::vector<unsigned char> getCommandData();
		std::string getCommandLog() const;
		void constructData();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		RequestOccCall (const RequestOccCall&){};
		RequestOccCall& operator= (const RequestOccCall& ){};

		static const char COMMAND;
		static const char LENGTH;
		static const char REQUEST_ORIGIN;
	};
}

#endif