#ifndef PLAYBACKCTRLCOMMAND_H
#define PLAYBACKCTRLCOMMAND_H

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

namespace TA_IRS_App
{
    class COEVideoOutput;
    class PlaybackCtrlCommand : public AbstractCOERequestCommand
    {
        public:
            enum CommandType
            {
                FAST_FORWARD_1_STEP = 1,
                FAST_BACKWARD_1_STEP,
                RESUME_PLAYBACK,
                PAUSE_PLAYBACK
            };
            PlaybackCtrlCommand(COEVideoOutput& monitor, CommandType cmd);
            virtual ~PlaybackCtrlCommand();
            virtual bool processResponse(const std::vector< unsigned char >& data);

        private:
            unsigned char m_expectedResponseCmd;
            COEVideoOutput& m_monitor;
    };
}

#endif
