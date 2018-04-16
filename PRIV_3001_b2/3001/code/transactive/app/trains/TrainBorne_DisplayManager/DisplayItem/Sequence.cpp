/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_display_manager/src/Sequence.cpp $
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


#include "Sequence.h"
#include "Camera.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <sstream>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;


namespace TA_IRS_App
{

    Sequence::Sequence( unsigned long entityKey,
                        const std::string& name,
                        const std::string& description//,
                        //const std::string& group 
						)
      : VideoInput(entityKey, name, description, "" ),
        m_dwellTime(0),
        m_cameras(),
        m_readOnly(false),
        m_sequenceDescription("") // TD16691
    {
        FUNCTION_ENTRY( "Sequence" );
        FUNCTION_EXIT;
    }


    unsigned short Sequence::getDwellTime()
    {
        FUNCTION_ENTRY( "getDwellTime" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_dwellTime;
    }


    void Sequence::setDwellTime(unsigned short dwellTime)
    {
        FUNCTION_ENTRY( "setDwellTime" );

        ThreadGuard guard( m_lock );

        m_dwellTime = dwellTime;

        FUNCTION_EXIT;
    }


    bool Sequence::getReadOnly()
    {
        FUNCTION_ENTRY( "getReadOnly" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_readOnly;
    }


    void Sequence::setReadOnly(bool readOnly)
    {
        FUNCTION_ENTRY( "setReadOnly" );

        ThreadGuard guard( m_lock );

        m_readOnly = readOnly;

        FUNCTION_EXIT;
    }


    // TD16691
    std::string Sequence::getSequenceDescription()
    {
        FUNCTION_ENTRY( "getSequenceDescription" );

        ThreadGuard guard(m_lock);

        FUNCTION_EXIT;
        return m_sequenceDescription;
    }


    void Sequence::setSequenceDescription(std::string sequenceDescription)
    {
        FUNCTION_ENTRY( "setSequenceDescription" );

        ThreadGuard guard(m_lock);

        m_sequenceDescription = sequenceDescription;

        FUNCTION_EXIT;
    }


    // TD16691

    std::list<Camera*> Sequence::getCameras()
    {
        FUNCTION_ENTRY( "getCameras" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_cameras;
    }


    void Sequence::setCameras(std::list<Camera*> cameras)
    {
        FUNCTION_ENTRY( "setCameras" );

        ThreadGuard guard( m_lock );

        m_cameras = cameras;

        FUNCTION_EXIT;
    }


    std::string Sequence::getDetails()
    {
        FUNCTION_ENTRY( "getDetails" );

        // reentrant lock - parent class uses the same lock
        ThreadGuard guard( m_lock );

        // get the item details
        std::ostringstream detailsStr;
        detailsStr << VideoInput::getDetails() << " : " << getSequenceDescription() << "\r\n";

        if (m_readOnly)
        {
            detailsStr << "Read Only Sequence" << "\r\n";
        }

        detailsStr << "Dwell time: " << m_dwellTime << "\r\n";

        // details of cameras
        int i = 1;
        for (std::list<Camera*>::iterator iter = m_cameras.begin();
        iter != m_cameras.end(); iter++, i++)
        {
            TA_ASSERT(*iter != NULL, "This sequence has a NULL camera");

            detailsStr << i << ": ";

            // even padding
            if (i < 10)
            {
                detailsStr << " ";
            }

            detailsStr << (*iter)->getDetails() << "\r\n";
        }

        if (m_cameras.size() == 0)
        {
            detailsStr << "Empty Sequence" << "\r\n";
        }

        FUNCTION_EXIT;
        return detailsStr.str();
    }


}


