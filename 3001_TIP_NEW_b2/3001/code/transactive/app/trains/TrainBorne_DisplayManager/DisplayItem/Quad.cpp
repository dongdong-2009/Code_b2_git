/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_display_manager/src/Quad.cpp $
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


#include "Quad.h"
#include "Camera.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <sstream>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{

    Quad::Quad( unsigned long entityKey,
                const std::string& name,
                const std::string& description,
                const std::string& group,
				const std::string& address,
				const std::string& cameras)
      : VideoInput(entityKey, name, description, group ),
	  m_address(address)
    {
        FUNCTION_ENTRY( "Quad" );

        // make sure the list has 4 items
        m_cameras.push_back(NULL);
        m_cameras.push_back(NULL);
        m_cameras.push_back(NULL);
        m_cameras.push_back(NULL);
		char *pos,*orgpos;
		char buffer[256];
		strcpy(buffer,(char*)cameras.c_str());
		char strCamera[256];
		orgpos = buffer;
		memset(m_camerasAddress,0,sizeof(m_camerasAddress));
		
		for(int i=0;i<4;i++)
		{
			memset(strCamera,0,sizeof(strCamera));
			pos = strstr(orgpos,",");
			strncpy(strCamera,orgpos,(pos!=NULL) ?( pos-orgpos):(buffer+cameras.size()-orgpos) );
			m_camerasAddress[i]= atoi(strCamera);
			if(pos!=NULL)
				orgpos=pos+1;
			else
				break;
		}
        FUNCTION_EXIT;
    }


    std::vector<Camera*> Quad::getCameras()
    {
        FUNCTION_ENTRY( "getCameras" );

        ThreadGuard guard( m_lock );
        TA_ASSERT(m_cameras.size() == 4, "There must be 4 cameras associated with a quad");

        FUNCTION_EXIT;
        return m_cameras;
    }


    void Quad::setTopLeft(Camera* topLeft)
    {
        FUNCTION_ENTRY( "setTopLeft" );

        ThreadGuard guard( m_lock );
        TA_ASSERT(m_cameras.size() == 4, "There must be 4 cameras associated with a quad");
        m_cameras[0] = topLeft;

        FUNCTION_EXIT;
    }


    void Quad::setTopRight(Camera* topRight)
    {
        FUNCTION_ENTRY( "setTopRight" );

        ThreadGuard guard( m_lock );
        TA_ASSERT(m_cameras.size() == 4, "There must be 4 cameras associated with a quad");
        m_cameras[1] = topRight;

        FUNCTION_EXIT;
    }


    void Quad::setBottomLeft(Camera* bottomLeft)
    {
        FUNCTION_ENTRY( "setBottomLeft" );

        ThreadGuard guard( m_lock );
        TA_ASSERT(m_cameras.size() == 4, "There must be 4 cameras associated with a quad");
        m_cameras[2] = bottomLeft;

        FUNCTION_EXIT;
    }


    void Quad::setBottomRight(Camera* bottomRight)
    {
        FUNCTION_ENTRY( "setBottomRight" );

        ThreadGuard guard( m_lock );
        TA_ASSERT(m_cameras.size() == 4, "There must be 4 cameras associated with a quad");
        m_cameras[3] = bottomRight;

        FUNCTION_EXIT;
    }


    std::string Quad::getDetails()
    {
        FUNCTION_ENTRY( "getDetails" );

        // reentrant lock - parent class uses the same lock
        ThreadGuard guard( m_lock );

        TA_ASSERT(m_cameras.size() == 4, "There must be 4 cameras associated with a quad");

        // get the item details
        std::ostringstream detailsStr;
        detailsStr << VideoInput::getDetails() << ":" << "\r\n";

        // details of cameras
        for (unsigned int i = 0; i < m_cameras.size(); i++)
        {
            detailsStr << (i+1) << ":  ";

            if (m_cameras[i] != NULL)
            {
                detailsStr << m_cameras[i]->getDetails() << "\r\n";
            }
            else
            {
                detailsStr << "No current assignment" << "\r\n";
            }
        }

        FUNCTION_EXIT;
        return detailsStr.str();
    }


}


