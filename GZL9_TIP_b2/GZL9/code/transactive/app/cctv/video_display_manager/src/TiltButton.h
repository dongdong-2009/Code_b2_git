#pragma once

#include "PTZButton.h"

namespace TA_IRS_App
{
    class CTiltButton : public CPTZButton
    {
        public:
			CTiltButton(unsigned short tiltDirection);
            virtual bool handleButtonUp ();
            virtual bool handleButtonDown();

        public:
            ~CTiltButton ( void );
        private:
            unsigned short m_tiltDirection;
    };
}
