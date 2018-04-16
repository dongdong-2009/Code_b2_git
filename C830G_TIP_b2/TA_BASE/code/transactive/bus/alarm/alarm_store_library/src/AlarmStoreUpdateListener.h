/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmStoreUpdateListener.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// AlarmStoreUpdateListener.h: interface for the AlarmStoreUpdateListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSTOREUPDATELISTENER_H__F177FE29_4996_4A04_A801_D51E8AA98B16__INCLUDED_)
#define AFX_ALARMSTOREUPDATELISTENER_H__F177FE29_4996_4A04_A801_D51E8AA98B16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/security/security_utilities/src/SecurityUtilities.h"

namespace TA_Base_Bus
{

    class AlarmStoreUpdateListener  
    {
    public:
        AlarmStoreUpdateListener() {};
        virtual ~AlarmStoreUpdateListener() {};

        SessionUpdateSubject& getSessionUpdateSubject() {return m_sessionUpdater;};
        DutyUpdateSubject& getDutyUpdateSubject() {return m_dutyUpdater;};

    private:
    
        SessionUpdateSubject    m_sessionUpdater;
        DutyUpdateSubject       m_dutyUpdater;

    };
}

#endif // !defined(AFX_ALARMSTOREUPDATELISTENER_H__F177FE29_4996_4A04_A801_D51E8AA98B16__INCLUDED_)
