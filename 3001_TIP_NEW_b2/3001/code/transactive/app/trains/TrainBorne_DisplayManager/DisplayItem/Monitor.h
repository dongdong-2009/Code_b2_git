/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_display_manager/src/TrainBorneMonitor.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(_TRAINBORNE_MONITOR_H)
#define _TRAINBORNE_MONITOR_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoOutput.h"

namespace TA_IRS_App
{

    class TrainBorneMonitor : public VideoOutput
    {
    public:

        TrainBorneMonitor( unsigned long entityKey,
                 const std::string& name,
                 const std::string& description,
				 const std::string& addr,
				 unsigned int displayOder,
				 unsigned long consoleKey,
				 bool isOverallMonitor
                );


        virtual ~TrainBorneMonitor() {}


        /**
          * isConsoleTrainBorneMonitor
          *
          * Gets whether this is a console monitor or not
          *
          * @return true if this is a console monitor
          */
        virtual bool isConsoleMonitor();
		virtual bool isOverAllMonitor();
		virtual unsigned int getDisplayOrder();
		virtual unsigned long getConsoleKey();
		virtual std::string   getAddress();
    protected:

        bool m_isConsoleMonitor;
		unsigned int m_displayOrder;
		unsigned long m_consoleKey;
		std::string m_address;
		bool m_isOverallMonitor;
    private:

        // These arent needed
        TrainBorneMonitor();
        TrainBorneMonitor( const TrainBorneMonitor& );
        TrainBorneMonitor& operator=( const TrainBorneMonitor& );

    };
}

#endif // !defined(TrainBorneMonitor_H)

