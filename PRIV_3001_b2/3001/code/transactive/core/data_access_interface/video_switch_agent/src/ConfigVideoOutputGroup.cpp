/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/ConfigVideoOutputGroup.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ConfigVideoOutputGroup is a concrete implementation of IConfigVideoOutputGroup, which is in turn 
  * an implementation of IVideoOutputGroup. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for VideoOutputGroups.
  */

#include <memory>

#include "core/data_access_interface/video_switch_agent/src/ConfigVideoOutputGroup.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
    //
    // ConfigVideoOutputGroup
    //
    ConfigVideoOutputGroup::ConfigVideoOutputGroup( unsigned long key )
        : m_videoOutputGroupHelper( std::auto_ptr< VideoOutputGroupHelper >( new VideoOutputGroupHelper( key ) ) )
    {
        // Do nothing.
    }


    //
    // setVideoOutputs
    //
    void ConfigVideoOutputGroup::setVideoOutputs( const std::map< unsigned short, unsigned long >& videoOutputs )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigVideoOutputGroup::setType");

        TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The VideoOutputGroupHelper pointer is null." );
        m_videoOutputGroupHelper->setVideoOutputs( videoOutputs );

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigVideoOutputGroup::setType");
    }

    
    //
    // getKey
    //
    unsigned long ConfigVideoOutputGroup::getKey()
    {
        TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The VideoOutputGroupHelper pointer is null." );
        return m_videoOutputGroupHelper->getKey();
    }


    //
    // getVideoOutputs
    //
    std::map< unsigned short, unsigned long > ConfigVideoOutputGroup::getVideoOutputs()
    {
        TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The VideoOutputGroupHelper pointer is null." );
        return m_videoOutputGroupHelper->getVideoOutputs();
    }


    //
    // invalidate
    //
    void ConfigVideoOutputGroup::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigVideoOutputGroup::invalidate");

        TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The VideoOutputGroupHelper pointer is null." );
        m_videoOutputGroupHelper->invalidate();

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigVideoOutputGroup::invalidate");
    }


    //
    // deleteThisObject
    //
    void ConfigVideoOutputGroup::deleteThisObject()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigVideoOutputGroup::deleteThisObject");

        TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The VideoOutputGroupHelper pointer is null." );

        // Delete the VideoOutputGroup from the database.

        m_videoOutputGroupHelper->deleteVideoOutputGroup();
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "About to delete VideoOutputGroupHelper pointer m_videoOutputGroupHelper." );

        m_videoOutputGroupHelper = std::auto_ptr< VideoOutputGroupHelper >( 0 );

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigVideoOutputGroup::deleteThisObject");
    }


    //
    // applyChanges
    //
    void ConfigVideoOutputGroup::applyChanges()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigVideoOutputGroup::applyChanges");

        TA_ASSERT( 0 != m_videoOutputGroupHelper.get(), "The VideoOutputGroupHelper pointer is null." );
        m_videoOutputGroupHelper->writeVideoOutputGroupData();

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigVideoOutputGroup::applyChanges");
    }

} // closes TA_IRS_Core

