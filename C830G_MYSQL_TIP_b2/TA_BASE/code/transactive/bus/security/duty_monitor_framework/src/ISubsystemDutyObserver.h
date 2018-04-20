#if !defined(_ISUBSYSTEMDUTYOBSERVER_H__944F4C3E_9D92_45BA_A6DF_391EF7930699__INCLUDED_)
#define _ISUBSYSTEMDUTYOBSERVER_H__944F4C3E_9D92_45BA_A6DF_391EF7930699__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_monitor_framework/src/ISubsystemDutyObserver.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class should be implemented by clients who want to be notified when  
  * SubsystemDutyMatrix contents change due to subsystem duty add/remove events.
  *
  */

namespace TA_Base_Bus
{
    class ISubsystemDutyObserver : public SeObserver<SubsystemDutyMatrix>
    {
    public:
        ISubsystemDutyObserver(SubsystemDutyMatrix& subsystemDutyMatrix);
        virtual ~ISubsystemDutyObserver();

        /**
          * Implements SeObserver::update(). Gets the update from
          * SubsystemDutyMatrix and calls dutyChanged() with it.
          *
          */
        virtual void update(SubsystemDutyMatrix& subsystemDutyMatrix);

        /**
          * This is called when a subsystem duty change occurs. 
          * Observers should implement this method.
          *
          */
        virtual void dutyChanged(const SubsystemDutyUpdate& dutyUpdate) = 0;
    };
}

#endif // !defined(_ISUBSYSTEMDUTYOBSERVER_H__944F4C3E_9D92_45BA_A6DF_391EF7930699__INCLUDED_)
