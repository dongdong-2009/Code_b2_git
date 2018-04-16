/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/DAIVideoOutputGroup.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * DAIVideoOutputGroup is an implementation of IVideoOutputGroup. It holds the data specific to a 
  * VideoOutputGroup mapping entry in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/video_switch_agent/src/DAIVideoOutputGroup.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    namespace TA_VideoSwitchAgentDAI
    {
        //
        // DAIVideoOutputGroup
        //
        DAIVideoOutputGroup::DAIVideoOutputGroup( unsigned long key )
            : m_videoOutputGroupHelper( std::auto_ptr< VideoOutputGroupHelper >( new VideoOutputGroupHelper( key ) ) )
        {
            // Do nothing.
        }


        //
        // getKey
        //
        unsigned long DAIVideoOutputGroup::getKey()
        {
            TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The VideoOutputGroupHelper pointer is null." );
            return m_videoOutputGroupHelper->getKey();
        }

    
        //
        // getVideoOutputs
        //
        std::map< unsigned short, unsigned long > DAIVideoOutputGroup::getVideoOutputs()
        {
            TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The DAIVideoOutputGroupHelper pointer is null." );
            return m_videoOutputGroupHelper->getVideoOutputs();
        }


        //
        // invalidate
        //
        void DAIVideoOutputGroup::invalidate()
        {
            TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The DAIVideoOutputGroupHelper pointer is null." );
            m_videoOutputGroupHelper->invalidate();
        }

    } // TA_DAIVideoOutputGroupDAI

} // closes TA_IRS_Core
