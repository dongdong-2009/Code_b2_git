#ifndef PTZCTRLCOMMAND_H
#define PTZCTRLCOMMAND_H

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

namespace TA_IRS_App
{
    class COECamera;
    class PtzCtrlCommand : public AbstractCOERequestCommand
    {
        public:
            enum CommandType
            {
                PAN_LEFT = 1,
                PAN_RIGHT,
                TILT_UP,
                TILT_DOWN,
                ZOOM_IN,
                ZOOM_OUT,
                RETURN_DEFAULT
            };
            PtzCtrlCommand(COECamera& camera, CommandType type);
            virtual ~PtzCtrlCommand();
            virtual bool processResponse(const std::vector< unsigned char >& data);

        private:
            CommandType m_type;
            unsigned char m_expectedResponseCmd;
            COECamera& m_camera;
    };
}

#endif
