/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Monitor.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
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

#include "app/cctv/video_display_manager/src/VideoOutput.h"

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
					const std::string& m_address,
					bool isQuad);
            virtual ~Monitor();
            virtual bool isConsoleMonitor();
			virtual bool isQuad();
            virtual unsigned long getConsoleKey();
            virtual int getDisplayOrder();
			virtual std::string getAddress();
			bool isVirtualQuadMonitor();
			void setSingleWallMonitor(bool isSingle);
			bool isSingleWallMonitor();
        protected:
            bool m_isConsoleMonitor;
            unsigned long m_consoleKey;
            int m_order;
			std::string m_address;
			bool m_isQuad;
			bool m_isVirtualQuadMon;
			bool m_isSingleWallMonitor;
    };
}

#endif // !defined(Monitor_H)

