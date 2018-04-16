
#ifndef _TEST_CALL_COMMAND_H_
#define _TEST_CALL_COMMAND_H_
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimsCommand.h"

namespace TA_IRS_App
{
	class TestCallCommand : public TimsCommand
	{
	public:
		TestCallCommand();
		~TestCallCommand();

		//Interface for TimsCommand
		int getCommandType() const;
		std::vector<unsigned char> getCommandData();
		std::string getCommandLog() const;
		void constructData();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		TestCallCommand( const TestCallCommand& ){};
		TestCallCommand& operator= (const TestCallCommand& ){};

		static const char COMMAND;
		static const char LENGTH;
		static const char STATUS12;
	};
}
#endif