/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/AbstractFailoverable.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// AbstractFailoverable.h: interface for the AbstractFailoverable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AbstractFailoverable_INCLUDED)
#define AbstractFailoverable_INCLUDED

#include "core/synchronisation/src/Condition.h"

class AbstractFailoverable  
{

public:
    AbstractFailoverable()
        : m_isEnabled(false)
    {
    }


    virtual ~AbstractFailoverable()
    {
    }

    virtual void setControl(bool isOnStartup=false)
    {
        // Implicitly threadsafe
        m_isEnabled=true;
        m_enableSignal.signal();
    }
    virtual void setMonitor(bool isOnStartup=false)
    {
        // Implicitly threadsafe
        m_isEnabled=false;
        m_enableSignal.signal();
    }

protected:
    volatile bool m_isEnabled;

    //TD4303, to notify derived class that enable changed
    TA_Base_Core::Condition     m_enableSignal;
    
};


#endif // !defined(AbstractFailoverable_INCLUDED)
