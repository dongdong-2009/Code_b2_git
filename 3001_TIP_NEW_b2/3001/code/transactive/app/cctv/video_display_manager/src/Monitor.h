/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/Monitor.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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

        Monitor( unsigned long entityKey,
                 const std::string& name,
                 const std::string& description,
				 const std::string& addr,
				 unsigned int  displayOrder,
				 unsigned long consoleKey,
                 bool isOverallMonitor);


        virtual ~Monitor() {}


        /**
          * isConsoleMonitor
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
        Monitor();
        Monitor( const Monitor& );
        Monitor& operator=( const Monitor& );

    };
}

#endif // !defined(Monitor_H)

