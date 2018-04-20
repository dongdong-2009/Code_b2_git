#if !defined(_ISESSIONOBSERVER_H__634E9F59_45D3_44FC_8FFF_F24A87318DED__INCLUDED_)
#define _ISESSIONOBSERVER_H__634E9F59_45D3_44FC_8FFF_F24A87318DED__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_monitor_framework/src/ISessionObserver.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class should be implemented by clients who want to be notified when  
  * SessionCache contents change due to operator login, logout and override events.
  *
  */

namespace TA_Base_Bus
{
    class ISessionObserver : public SeObserver<SessionCache>
    {
    public:
        ISessionObserver(SessionCache& sessionCache);
        virtual ~ISessionObserver();

        /**
          * Implements SeObserver::update(). Gets the update from
          * SessionCache and calls sessionChanged() with it.
          *
          */
        virtual void update(SessionCache& sessionCache);

        /**
          * This is called when a session change occurs. 
          * Observers should implement this method.
          *
          */
        virtual void sessionChanged(const SessionUpdate& sessionUpdate) = 0;
    };
}

#endif // !defined(_ISESSIONOBSERVER_H__634E9F59_45D3_44FC_8FFF_F24A87318DED__INCLUDED_)
