/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasReading.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#include "ace/Guard_T.h"
#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"

#include "bus/modbus_comms/src/DataBlock.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/IPasDataProcessor.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/PasReading.h"

namespace TA_IRS_App
{

PasReading::PasReading( const bool bAtOcc, IPasDataProcessor& refProcessor, 
    const int nDataStartAddress, const int  nDataEndAddress ) : 
PasTransaction( bAtOcc, NULL ),
m_refProcessor( refProcessor ),
m_nDataStartAddress( nDataStartAddress ),
m_nDataEndAddress( nDataEndAddress )
{
}

PasReading::~PasReading()
{
}

int PasReading::executeImp( PasConnectionAgency& refConnection )
{
    TA_Base_Bus::DataBlock<unsigned short> dbPollZones( m_nDataStartAddress, m_nDataEndAddress );

    if ( !refConnection.readFromPasWithRetry( dbPollZones ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to read data from PAS" );
        return TRANSACTION_FAILED;
    }

    try
    {
        m_refProcessor.processData( dbPollZones );
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when calling processData for PasReading processor" );
    }

    return TRANSACTION_SUCCESS;
}

int PasReading::executeOccImp( PasConnectionAgency& refConnection )
{
    return executeImp( refConnection );
}

int PasReading::executeStnImp( PasConnectionAgency& refConnection )
{
    return executeImp( refConnection );
}

}
