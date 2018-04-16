#if !defined(_IREGIONDUTYOBSERVER_H__A4184045_F2C9_423F_968D_EFAA5E2BD9C8__INCLUDED_)
#define _IREGIONDUTYOBSERVER_H__A4184045_F2C9_423F_968D_EFAA5E2BD9C8__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_monitor_framework/src/IRegionDutyObserver.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class should be implemented by clients who want to be notified when  
  * RegionDutyMatrix contents change due to region duty add/remove events.
  *
  */

namespace TA_Base_Bus
{
    class IRegionDutyObserver : public SeObserver<RegionDutyMatrix>
    {
    public:
        IRegionDutyObserver(RegionDutyMatrix& regionDutyMatrix);
        virtual ~IRegionDutyObserver();

        /**
          * Implements SeObserver::update(). Gets duty update from
          * RegionDutyMatrix and calls dutyChanged() with it.
          *
          */
        virtual void update(RegionDutyMatrix& regionDutyMatrix);

        /**
          * This is called when a region duty change occurs. 
          * Observers should implement this method.
          *
          */
        virtual void dutyChanged(const RegionDutyUpdate& dutyUpdate) = 0;
    };
}

#endif // !defined(_IREGIONDUTYOBSERVER_H__A4184045_F2C9_423F_968D_EFAA5E2BD9C8__INCLUDED_)
