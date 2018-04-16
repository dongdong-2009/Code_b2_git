/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSMessage.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */


#include "ace/OS.h"

#include "app/signs/pids_agent/src/PIDSMessage.h"
#include "app/signs/pids_agent/src/PIDSManager.h"

#include <iomanip>

namespace TA_IRS_App
{

	PIDSMessage::PIDSMessage() : m_dataIsSet( false )
	{
	}


    PIDSMessage::~PIDSMessage()
	{
	}


    void PIDSMessage::setData()
    {
    }
}
