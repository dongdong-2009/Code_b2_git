/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/Sequence.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Sequence is an implementation of ISequence. It holds the data specific to a 
  * Sequence mapping entry in the database, and allows read-only access to that data.
  *
  * Note: The dwell time for Sequences can be retrieved using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#include "core/data_access_interface/video_switch_agent/src/Sequence.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    namespace TA_VideoSwitchAgentDAI
    {
        //
        // Sequence
        //
        Sequence::Sequence( unsigned long key )
            : m_sequenceHelper( std::auto_ptr< SequenceHelper >( new SequenceHelper( key ) ) )
        {
            // Do nothing.
        }


        //
        // getKey
        //
        unsigned long Sequence::getKey()
        {
            TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
            return m_sequenceHelper->getKey();
        }


        //
        // getVideoOutputs
        //
        std::map< unsigned short, unsigned long > Sequence::getVideoInputs()
        {
            TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
            return m_sequenceHelper->getVideoInputs();
        }


        //
        // invalidate
        //
        void Sequence::invalidate()
        {
            TA_ASSERT( 0 != m_sequenceHelper.get(), "The SequenceHelper pointer is null." );
            m_sequenceHelper->invalidate();
        }

    } // TA_SequenceDAI

} // closes TA_IRS_Core
