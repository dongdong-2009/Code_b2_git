#include "app/cctv/video_switch_agent/protocols/COE/src/RecordingPlaybackCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/PtzCtrlCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/PlaybackCtrlCommand.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"

namespace TA_IRS_App
{
    namespace TA_COE
    {
        IAgentRequestCommand* createSwitchInputToOutputCommand(unsigned long videoInputEntityKey,
                                                               COEVideoOutput& output);

        IAgentRequestCommand* createSwitchQuadInputsCommand(unsigned long topLeftVideoInput,
                                                            unsigned long topRightVideoInput,
                                                            unsigned long bottomLeftVideoInput,
                                                            unsigned long bottomRightVideoInput,
                                                            COEVideoOutput& monitor);

        IAgentRequestCommand* createSetSequenceConfigCommand(unsigned short dwellTime,
                                                             const std::vector< unsigned long >& videoInputs,
                                                             COESequence& sequence);

        IAgentRequestCommand* createSummaryPollingCommand();

        IAgentRequestCommand* createRecordingPlayBackCommand(unsigned long cameraKey,
                                                             const RecordingPlaybackCommand::Time& startTime,
                                                             const RecordingPlaybackCommand::Time& endTime,
                                                             COEVideoOutput& monitor);

        IAgentRequestCommand* createPlayBackCtrlCommand(PlaybackCtrlCommand::CommandType cmd,
                                                        COEVideoOutput& monitor);

        IAgentRequestCommand* createPtzCtrlCommand(PtzCtrlCommand::CommandType cmdType,
			                                       COECamera& camera);

        RecordingPlaybackCommand::Time convertCorbaTimeToRecordingComdTime(const TA_Base_Bus::VideoOutputCorbaDef::Time& time);
        PlaybackCtrlCommand::CommandType convertCorbaCmdToPlaybackCtrlCmdType(const TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd& cmd);
		PtzCtrlCommand::CommandType convertCorbaCmdToPtzCtrlCmdType(const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd cmd);
    }
}

