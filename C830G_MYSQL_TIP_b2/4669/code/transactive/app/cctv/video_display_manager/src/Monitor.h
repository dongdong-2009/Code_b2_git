/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/Monitor.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
                 bool isConsoleMonitor );


        virtual ~Monitor() {}


        /**
          * isConsoleMonitor
          *
          * Gets whether this is a console monitor or not
          *
          * @return true if this is a console monitor
          */
        virtual bool isConsoleMonitor();


    protected:

        bool m_isConsoleMonitor;

    private:

        // These arent needed
        Monitor();
        Monitor( const Monitor& );
        Monitor& operator=( const Monitor& );

    };
}

#endif // !defined(Monitor_H)

