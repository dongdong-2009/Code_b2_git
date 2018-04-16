/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Monitor.h $
  * @author:  Adam Radics
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/11/18 19:40:33 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(Monitor_H)
#define Monitor_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoOutput.h"

namespace TA_IRS_App
{

    class Monitor : public VideoOutput
    {
        public:

            Monitor(unsigned long entityKey,
                    const std::string& name,
                    const std::string& description,
                    bool isConsoleMonitor,
                    unsigned long consoleKey,
                    int order,
					const std::string& m_address);
            virtual ~Monitor() {}
            /**
              * isConsoleMonitor
              *
              * Gets whether this is a console monitor or not
              *
              * @return true if this is a console monitor
              */
            virtual bool isConsoleMonitor();
            virtual unsigned long getConsoleKey();
            virtual int getDisplayOrder();
			virtual std::string getAddress();
			bool isQuadMonitor();
        protected:

            bool m_isConsoleMonitor;
            unsigned long m_consoleKey;
            int m_order;
			std::string m_address;
			bool m_isQuadMonitor;
    };
}

#endif // !defined(Monitor_H)

