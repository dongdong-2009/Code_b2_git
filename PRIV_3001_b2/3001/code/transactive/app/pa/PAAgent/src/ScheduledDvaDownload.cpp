/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ScheduledDvaDownload.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "bus/modbus_comms/src/DataBlock.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/IPasDataProcessor.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/ScheduledDvaDownload.h"

namespace TA_IRS_App
{

ScheduledDvaDownload::ScheduledDvaDownload( const bool bAtOcc, ITransactionOwner* pOwner ) : 
PasTransaction( bAtOcc, pOwner ),
m_mtxEventIds(),
m_mapEventIdAvailable(),
m_usMinEventId( 1u )
{
}

ScheduledDvaDownload::~ScheduledDvaDownload()
{
}

int ScheduledDvaDownload::executeOccImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute ScheduledDvaDownload" );

    TA_Base_Bus::DataBlock<unsigned short> dbScheduleBuff;
    int nStartAddr = 0;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxEventIds );
        
        for ( std::map<unsigned short, bool>::iterator itLoop = m_mapEventIdAvailable.begin(); 
            m_mapEventIdAvailable.end() != itLoop; ++itLoop )
        {
            nStartAddr =  PAS_OCC_SCHEDULE_DVA_BUFF_LENGTH * 
                static_cast<int>(itLoop->first - m_usMinEventId) + PAS_ADDRESS_OCC_SCHEDULE_ID_STATE;

            dbScheduleBuff.setStartAndLength( nStartAddr, PAS_SCHEDULE_ID_STATE_LENGTH );

            if ( !refConnection.readFromPasWithRetry( dbScheduleBuff ))
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read data from PAS" );
                return TRANSACTION_FAILED;
            }

            itLoop->second = ( dbScheduleBuff[nStartAddr] == PAS_SCHEDULE_ID_AVAILABLE );
        }
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScheduledDvaDownload execution ended" );

    return TRANSACTION_SUCCESS;
}

int ScheduledDvaDownload::executeStnImp( PasConnectionAgency& refConnection )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to execute ScheduledDvaDownload" );

    TA_Base_Bus::DataBlock<unsigned short> dbScheduleBuff;
    int nStartAddr = 0;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxEventIds );

        for ( std::map<unsigned short, bool>::iterator itLoop = m_mapEventIdAvailable.begin(); 
            m_mapEventIdAvailable.end() != itLoop; ++itLoop )
        {
            nStartAddr =  PAS_STN_SCHEDULE_DVA_BUFF_LENGTH * 
                static_cast<int>(itLoop->first - m_usMinEventId) + PAS_ADDRESS_STN_SCHEDULE_ID_STATE;

            dbScheduleBuff.setStartAndLength( nStartAddr, PAS_SCHEDULE_ID_STATE_LENGTH );

            if ( !refConnection.readFromPasWithRetry( dbScheduleBuff ))
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read data from PAS" );
                return TRANSACTION_CONNECTION_FAILURE;
            }

            itLoop->second = ( dbScheduleBuff[nStartAddr] == PAS_SCHEDULE_ID_AVAILABLE );
        }
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScheduledDvaDownload execution ended" );

    return TRANSACTION_SUCCESS;
}

bool ScheduledDvaDownload::isEventIdAvailable( const unsigned short usEventId )
{
    return false;
}

void ScheduledDvaDownload::setRefreshEventId( const unsigned short usEventId )
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxEventIds );

        m_mapEventIdAvailable.clear();
        m_mapEventIdAvailable.insert( std::map<unsigned short, bool>::value_type(usEventId, false) );
    }
}

void ScheduledDvaDownload::setMinEventId( const unsigned short usMin )
{
    m_usMinEventId = usMin;
}

void ScheduledDvaDownload::setRefreshEventIds( const unsigned short usMinId, const unsigned short usMaxId )
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxEventIds );

        m_mapEventIdAvailable.clear();
        for ( unsigned short usLoop = usMinId; usMaxId >= usLoop; ++usLoop )
        {
            m_mapEventIdAvailable.insert( std::map<unsigned short, bool>::value_type(usLoop, false) );
        }
    }
}

std::map<unsigned short, bool> ScheduledDvaDownload::getEventIdAvaliableMap()
{
    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxEventIds );
    return m_mapEventIdAvailable;
}

}
