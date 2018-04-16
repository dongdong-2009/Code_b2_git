#ifndef DUTYDBCACHEUPDATER_H_INCLUDED
#define DUTYDBCACHEUPDATER_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyDbCacheUpdater.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * Listens to online updates and refreshes DutyDbCache.
 */

namespace TA_Base_Bus
{
    class DutyDbCacheUpdater : public TA_Base_Core::IOnlineUpdatable
    {
    public:
		
		DutyDbCacheUpdater( DutyDbCacheImpl& dbCache );
		~DutyDbCacheUpdater();

		// Staisfies the IOnlineUpdatable interface
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    private:

        DutyDbCacheUpdater(const DutyDbCacheUpdater& );
        DutyDbCacheUpdater& operator=(const DutyDbCacheUpdater& );

		DutyDbCacheImpl& m_dbCache;
    };
}

#endif // DUTYDBCACHEUPDATER_H_INCLUDED
