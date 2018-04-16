// DVABroadcast.cpp: implementation of the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#include "app/pa/PAAgent/src/DVABroadcastStart.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/TriggeredDVABroadcast.h"

namespace TA_IRS_App
{

TriggeredDVABroadcast::TriggeredDVABroadcast( time_t tmStartTime, 
    TRIGGER_EVENT_TYPE eEventType, const std::string& strBroadcastId, unsigned long ulDvaMsgKey, 
    const std::vector<unsigned long>& lstZoneKeys, unsigned long ulCyclicTimes ) : 
m_strBroadcastId( strBroadcastId ),
m_eEventType( eEventType ),
m_ulDvaMsgKey( ulDvaMsgKey ),
m_lstZoneKeys( lstZoneKeys ),
m_ulCyclicTimes( ulCyclicTimes ),
m_tmStartTime( tmStartTime )
{
}

TriggeredDVABroadcast::TriggeredDVABroadcast( unsigned long ulSigEventType, unsigned long ulPlatFormType ):
m_ulSigEventType(ulSigEventType),
m_ulPlatFormType(ulPlatFormType)
{
}

TriggeredDVABroadcast::TriggeredDVABroadcast(const bool bOnFire):
m_bOnFire(bOnFire)
{
}

TriggeredDVABroadcast::~TriggeredDVABroadcast()
{
}

void TriggeredDVABroadcast::executeBroadcast()
{
    DVABroadcastStart newBroadcast( m_strBroadcastId );

    bool bSendToHardware = true;
    try
    {
        // update parameters
        newBroadcast.updatePriorityByEventType( m_eEventType );

        newBroadcast.updateStationZoneIdsByZoneKeys( m_lstZoneKeys );
        
        newBroadcast.updateDVAMsgIdByDVAMsgKey( m_ulDvaMsgKey );

        newBroadcast.updateCylicTimes( m_ulCyclicTimes );

        if ( newBroadcast.isParameterInvalid() )
        {
            bSendToHardware = false;
            std::string strBadParams = newBroadcast.getInvalidParameter();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Triggered broadcast can't be sent in case of bad parameter(s): %s", strBadParams.c_str() );
        }
    }
    catch(...)
    {
        bSendToHardware = false;
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
    }

    if ( bSendToHardware )
    {
        try
        {
            PASConnection::getInstance()->writeToHardwareImmediately( &newBroadcast );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
            throw;
        }
    }
}

void TriggeredDVABroadcast::executeSigBroadcast()
{
	ModbusPacket modbusPacket( "");

	bool bSendToHardware = true;
	try
	{
		// update parameters
        modbusPacket.setSigTriggedBit(m_ulSigEventType, m_ulPlatFormType);

		if ( modbusPacket.isParameterInvalid() )
		{
			bSendToHardware = false;
			std::string strBadParams = modbusPacket.getInvalidParameter();

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Triggered broadcast can't be sent in case of bad parameter(s): %s", strBadParams.c_str() );
		}
	}
	catch(...)
	{
		bSendToHardware = false;
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
	}

	if ( bSendToHardware )
	{
		try
		{
			PASConnection::getInstance()->writeToHardwareImmediately( &modbusPacket );
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
			throw;
		}
	}
}

void TriggeredDVABroadcast::executeFasBroadcast()
{
    ModbusPacket modbusPacket("");

    bool bSendToHardware = true;
    try
    {
        // update parameters
        modbusPacket.setFasTriggedBit(m_bOnFire);

        if (modbusPacket.isParameterInvalid())
        {
            bSendToHardware = false;
            std::string strBadParams = modbusPacket.getInvalidParameter();

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Triggered broadcast can't be sent in case of bad parameter(s): %s", strBadParams.c_str());
        }
    }
    catch (...)
    {
        bSendToHardware = false;
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
    }

    if (bSendToHardware)
    {
        try
        {
            PASConnection::getInstance()->writeToHardwareImmediately(&modbusPacket);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
            throw;
        }
    }
}

time_t TriggeredDVABroadcast::getStartTime()
{
    return m_tmStartTime;
}

std::string TriggeredDVABroadcast::getBroadcastId()
{
    return m_strBroadcastId;
}

}
