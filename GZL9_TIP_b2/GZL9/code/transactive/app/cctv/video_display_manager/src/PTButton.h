#pragma once

#include "PTZButton.h"

namespace TA_IRS_App
{
    class CPTButton : public CPTZButton
    {
        public:
            CPTButton ( unsigned short ptDirection);
            virtual bool handleButtonUp ();
            virtual bool handleButtonDown();
        public:
            ~CPTButton ();
        private:
            unsigned short m_ptDirection;
    };
}
