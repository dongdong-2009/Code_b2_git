#ifndef RECORDINGPLAYBACKCOMMAND_H
#define RECORDINGPLAYBACKCOMMAND_H

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

namespace TA_IRS_App
{
    class COEVideoOutput;
    class COECamera;
    class RecordingPlaybackCommand : public AbstractCOERequestCommand
    {
        public:
            struct Time
            {
                unsigned short year;
                unsigned char month;
                unsigned char day;
                unsigned char hour;
                unsigned char minute;
                unsigned char second;
            };
            RecordingPlaybackCommand(COECamera& camera,
                                     COEVideoOutput& monitor,
                                     const Time& startTime,
                                     const Time& endTime);
            virtual ~RecordingPlaybackCommand();
            virtual bool processResponse(const std::vector< unsigned char >& data);
        private:
            unsigned char m_expectedResponseCmd;
            COECamera& m_camera;
            COEVideoOutput& m_monitor;
    };
}

#endif
