/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/ConfigSequence.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ConfigSequence is a concrete implementation of IConfigSequence, which is in turn 
  * an implementation of ISequence. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for Sequences.
  *
  * Note: The dwell time for Sequences can be retrieved/set using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#include "core/data_access_interface/video_switch_agent/src/ConfigSequence.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    //
    // ConfigSequence
    //
    ConfigSequence::ConfigSequence( unsigned long key )
        : m_sequenceHelper( std::auto_ptr< SequenceHelper >( new SequenceHelper( key ) ) )
    {
        // Do nothing.
    }


    //
    // getKey
    //
    unsigned long ConfigSequence::getKey()
    {
        TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
        return m_sequenceHelper->getKey();
    }

    
    //
    // getVideoOutputs
    //
    std::map< unsigned short, unsigned long > ConfigSequence::getVideoInputs()
    {
        TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
        return m_sequenceHelper->getVideoInputs();
    }


    //
    // setVideoInputs
    //
    void ConfigSequence::setVideoInputs( const std::map< unsigned short, unsigned long >& videoInputs )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigSequence::setVideoInputs");

        TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
        m_sequenceHelper->setVideoInputs( videoInputs );

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigSequence::setVideoInputs");
    }

    
    //
    // invalidate
    //
    void ConfigSequence::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigSequence::invalidate");

        TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
        m_sequenceHelper->invalidate();

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigSequence::invalidate");
    }


    //
    // deleteThisObject
    //
    void ConfigSequence::deleteThisObject()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigSequence::deleteThisObject");

        TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );

        // Delete the Sequence from the database.

        m_sequenceHelper->deleteSequence();
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "About to delete SequenceHelper pointer m_sequenceHelper." );

        m_sequenceHelper = std::auto_ptr< SequenceHelper >( 0 );

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigSequence::deleteThisObject");
    }


    //
    // applyChanges
    //
    void ConfigSequence::applyChanges()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ConfigSequence::applyChanges");

        TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
        m_sequenceHelper->writeSequenceData();

        LOG_FUNCTION_EXIT( SourceInfo, "ConfigSequence::applyChanges");
    }

} // closes TA_IRS_Core

