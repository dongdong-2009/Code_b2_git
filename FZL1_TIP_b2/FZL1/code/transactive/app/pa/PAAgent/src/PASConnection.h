/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/PASConnection.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(PASCONNECTION__INCLUDED_)
#define PASCONNECTION__INCLUDED_

#include <boost/shared_ptr.hpp>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/ModbusHelper/src/IModbusHelper.h"
#include "app/pa/PAAgent/src/CachedConfig.h"


// Asynchronous sending is no need yet
namespace TA_IRS_App
{

class ModbusPacket;

class PASConnection : public ICachedConfigObserver
{
public:
    
    /**
     * getInstance
	 *
	 * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
	static PASConnection* getInstance();

    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();

    virtual ~PASConnection();

    bool writeToHardwareImmediately( ModbusPacket* pModbusPacket );

    // ICachedConfigObserver interface
    virtual void processCachedConfigUpdate(CachedConfig::ECachedConfigItemKey key);

    void logDataBlockToBinaryString( int nFileLine, TA_Base_Bus::DataBlock<WORD>& dbwToBeLoged );

private:
    void createModbusHelper();

protected:
    static PASConnection*                               m_me;   
    static TA_Base_Core::ReEntrantThreadLockable             m_lock;

    boost::shared_ptr<TA_Base_Bus::IModbusHelper>            m_pModbusHelper;

private:
    PASConnection();
    PASConnection( const PASConnection& refCopy );
    const PASConnection& operator=( const PASConnection& refCopy );
};

} // namespace TA_IRS_App

#endif // !defined(PASConnection_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

