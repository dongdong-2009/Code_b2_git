/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/control_station/src/SoundObserver.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// SoundObserver.cpp: implementation of the SoundObserver class.
//
//////////////////////////////////////////////////////////////////////

#include "app/system_control/control_station/src/stdafx.h"
#include "app/system_control/control_station/src/controlstation.h"
#include "app/system_control/control_station/src/SoundObserver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SoundObserver::SoundObserver(CButtonST& silenceButton) : m_silenceButton(silenceButton)
{

}

SoundObserver::~SoundObserver()
{

}

void SoundObserver::setState(bool state)
{
    // If the state is true, then the state has just been set to "silenced", so want to "check" the silence button
    m_silenceButton.SetCheck(state);
}
