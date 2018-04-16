
#include "app/radio/radio_simulator/CommandProcessorLib/src/TestCallCommand.h"
#include "app/radio/radio_simulator/TrainSimulator/SimulatorConfig.h"

namespace TA_IRS_App
{
	const char TestCallCommand::COMMAND = 0x47;
	const char TestCallCommand::LENGTH  = 19;

	TestCallCommand::TestCallCommand()
	{

	}

	TestCallCommand::~TestCallCommand()
	{

	}

	int TestCallCommand::getCommandType() const
	{
		return static_cast<int>(COMMAND);
	}

	void TestCallCommand::constructData()
	{
		//make sure that the data is clear
		m_commandData.clear();

		//Get the library Version No. to be upgraded
		SimulatorConfig* config = SimulatorConfig::getInstance();
		config->ReadConfig();
		std::vector<int> currentLibVN = config->getCurrentLibVersion();
		std::vector<int> nextLibVN = config->getNextLibVersion();
		std::vector<int> preRecordedLibVN = config->getPreRecordedLibVersion();

		m_commandData.push_back(FLAG);
		m_commandData.push_back(COMMAND);
		m_commandData.push_back(m_trainID);
		m_commandData.push_back(LENGTH);
		m_commandData.push_back(ORIGIN);

		//For Current Lib Version Number
		m_commandData.push_back(currentLibVN.at(3));
		m_commandData.push_back(currentLibVN.at(2));
		m_commandData.push_back(currentLibVN.at(1));
		m_commandData.push_back(currentLibVN.at(0));

		//For Next Lib Version Number
		m_commandData.push_back(nextLibVN.at(3));
		m_commandData.push_back(nextLibVN.at(2));
		m_commandData.push_back(nextLibVN.at(1));
		m_commandData.push_back(nextLibVN.at(0));

		//For Pre-recorded Lib Version Number
		m_commandData.push_back(preRecordedLibVN.at(3));
		m_commandData.push_back(preRecordedLibVN.at(2));
		m_commandData.push_back(preRecordedLibVN.at(1));
		m_commandData.push_back(preRecordedLibVN.at(0));
	
	}

	std::string TestCallCommand::getCommandLog() const
	{
		std::string _ret = "Sending Test Call Command...";
		return _ret;
	}
}