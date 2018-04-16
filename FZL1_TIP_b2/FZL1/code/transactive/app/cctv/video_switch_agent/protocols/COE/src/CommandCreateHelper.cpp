#include "CommandCreateHelper.h"
#include "SwitchInputToOutputCommand.h"
#include "SwitchQuadInputsCommand.h"
#include "SetSequenceConfigCommand.h"
#include "SummaryPollingCommand.h"
#include "COESwitchManager.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include <boost/lexical_cast.hpp>

namespace TA_IRS_App
{
    namespace TA_COE
    {
        //
        // createSwitchInputToOutputCommand
        //
        IAgentRequestCommand* createSwitchInputToOutputCommand(unsigned long videoInputEntityKey,
                                                               COEVideoOutput& output)
        {
            SwitchInputToOutputCommand* command = new SwitchInputToOutputCommand(videoInputEntityKey, output);
            command->buildCommand();
            return command;
        }


        //
        // createSwitchQuadInputsCommand
        //
        IAgentRequestCommand* createSwitchQuadInputsCommand(unsigned long topLeftVideoInput,
                                                            unsigned long topRightVideoInput,
                                                            unsigned long bottomLeftVideoInput,
                                                            unsigned long bottomRightVideoInput,
                                                            COEVideoOutput& monitor)
        {
            return new SwitchQuadInputsCommand(topLeftVideoInput,
                                               topRightVideoInput,
                                               bottomLeftVideoInput,
                                               bottomRightVideoInput,
                                               monitor);
        }


        //
        // createSetSequenceConfigCommand
        //
        IAgentRequestCommand* createSetSequenceConfigCommand(unsigned short dwellTime,
                                                             const std::vector< unsigned long >& videoInputs,
                                                             COESequence& sequence)
        {
            return new SetSequenceConfigCommand(dwellTime,
                                                videoInputs,
                                                sequence);
        }


        //
        // createSummaryPollingCommand
        //
        IAgentRequestCommand* createSummaryPollingCommand()
        {
            return new SummaryPollingCommand();
        }

        IAgentRequestCommand* createRecordingPlayBackCommand(unsigned long cameraKey,
                                                             const RecordingPlaybackCommand::Time& startTime,
                                                             const RecordingPlaybackCommand::Time& endTime,
                                                             COEVideoOutput& monitor)
        {
            COECamera* camera = COESwitchManager::getInstance().getCamera(cameraKey);
			if(NULL == camera)
			{
				std::string errorMsg = std::string("Can not find camera ") + boost::lexical_cast<std::string>(cameraKey);
	            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, errorMsg));
			}
            return new RecordingPlaybackCommand(*camera,
                                                monitor,
                                                startTime,
                                                endTime);
        }

        IAgentRequestCommand* createPlayBackCtrlCommand(PlaybackCtrlCommand::CommandType cmd,
                                                        COEVideoOutput& monitor)
        {
            return new PlaybackCtrlCommand(monitor, cmd);
        }


        IAgentRequestCommand* createPtzCtrlCommand(PtzCtrlCommand::CommandType cmdType,
			                                       COECamera& camera)
		{
			return new PtzCtrlCommand(camera, cmdType);
		}

        RecordingPlaybackCommand::Time convertCorbaTimeToRecordingComdTime(const TA_Base_Bus::VideoOutputCorbaDef::Time& time)
        {
            RecordingPlaybackCommand::Time t;
            t.year = time.year;
            t.month = time.month;
            t.day = time.day;
            t.hour = time.hour;
            t.minute = time.minute;
            t.second = time.second;
            return t;
        }

        PlaybackCtrlCommand::CommandType convertCorbaCmdToPlaybackCtrlCmdType(const TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd& cmd)
        {
            switch(cmd)
            {
                case TA_Base_Bus::VideoOutputCorbaDef::FAST_FORWARD_1_STEP:
                    return PlaybackCtrlCommand::FAST_FORWARD_1_STEP;
                case TA_Base_Bus::VideoOutputCorbaDef::FAST_BACKWARD_1_STEP:
                    return PlaybackCtrlCommand::FAST_BACKWARD_1_STEP;
                case TA_Base_Bus::VideoOutputCorbaDef::RESUME_PLAYBACK:
                    return PlaybackCtrlCommand::RESUME_PLAYBACK;
                default:
                    return PlaybackCtrlCommand::PAUSE_PLAYBACK;
            }
        }

		PtzCtrlCommand::CommandType convertCorbaCmdToPtzCtrlCmdType(const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd cmd)
        {
            switch(cmd)
            {
				case TA_Base_Bus::CameraCorbaDef::PAN_LEFT_WX:
					return PtzCtrlCommand::PAN_LEFT;
				case TA_Base_Bus::CameraCorbaDef::PAN_RIGHT_WX:
					return PtzCtrlCommand::PAN_RIGHT;
				case TA_Base_Bus::CameraCorbaDef::TILT_UP_WX:
					return PtzCtrlCommand::TILT_UP;
				case TA_Base_Bus::CameraCorbaDef::TILT_DOWN_WX:
					return PtzCtrlCommand::TILT_DOWN;
				case TA_Base_Bus::CameraCorbaDef::ZOOM_IN_WX:
					return PtzCtrlCommand::ZOOM_IN;
				case TA_Base_Bus::CameraCorbaDef::ZOOM_OUT_WX:
					return PtzCtrlCommand::ZOOM_OUT;
				default:
					return PtzCtrlCommand::RETURN_DEFAULT;
            }
        }
    }
}