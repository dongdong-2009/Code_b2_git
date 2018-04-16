/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasTransaction.cpp $
  * @author:  HuangQi
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/02/10 16:54:21 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "ace/Guard_T.h"
#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"

#include "bus/modbus_comms/src/DataBlock.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/ITransactionOwner.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/PasTransaction.h"

// For PAS reason, they need to remove the step of set execution register, we leave a switch here
#define PAS_IGNORE_SET_EXECUTION_REGISTER

namespace TA_IRS_App
{

const int READ_EXECUTION_REGISTER_INTERVAL = 50; // millisecond

PasTransaction::PasTransaction( const bool bAtOcc, ITransactionOwner* pOwner ) : 
m_pOwner( pOwner ),
m_bNoOwner( pOwner == NULL ),
m_bAtOcc( bAtOcc ),
m_mtxOwnerLock()
{
}

PasTransaction::~PasTransaction()
{
}

bool PasTransaction::cancelOwnerIfRelated( ITransactionOwner& refOwner )
{
    if ( m_bNoOwner )
    {
        return false;
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxOwnerLock );

        if ( m_pOwner != &refOwner )
        {
            return false;
        }

        m_pOwner = NULL;
    }

    return true;
}

int PasTransaction::execute( PasConnectionAgency& refConnection )
{
    int nResult = TRANSACTION_NOT_EXECUTED;

    if ( !m_bNoOwner )
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxOwnerLock );
        if ( NULL == m_pOwner )
        {
            return nResult;
        }
    }

    try
    {
        // Blocking method which cost lots time
        if ( m_bAtOcc )
        {
            nResult = executeOccImp( refConnection );
        }
        else
        {
            nResult = executeStnImp( refConnection );
        }
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when calling PasTransaction::execute" );
    }

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxOwnerLock );
        if ( NULL != m_pOwner )
        {
            m_pOwner->transactionExecuted( nResult );
        }
    }

    return nResult;
}

bool PasTransaction::waitExecutionAcknowledged( 
    PasConnectionAgency& refConnection, const unsigned long ulMilSecTimeout )
{
    bool bWaitSuccess = false;
    TA_Base_Bus::DataBlock<unsigned short> dbExecutionRegister( 
        PAS_ADDRESS_EXECUTION_REGISTER, PAS_ADDRESS_EXECUTION_REGISTER );
    ACE_Time_Value atmReadInterval;
    atmReadInterval.msec( READ_EXECUTION_REGISTER_INTERVAL );

    ACE_Time_Value atmStartTime = ACE_OS::gettimeofday();
    ACE_Time_Value atmCostTime = ACE_Time_Value::zero;
    while ( !bWaitSuccess )
    {
        // Sleep first may easy get acknowledge at the first time
        ACE_OS::sleep( atmReadInterval );

        if ( refConnection.readFromPasWithRetry( dbExecutionRegister ))
        {
            if ( 0 == dbExecutionRegister[PAS_ADDRESS_EXECUTION_REGISTER] )
            {
                bWaitSuccess = true;
                break;
            }
        }

        atmCostTime = ACE_OS::gettimeofday() - atmStartTime;
        if ( ulMilSecTimeout <= atmCostTime.msec() )
        {
            bWaitSuccess = false;
            break;
        }
    }

    return bWaitSuccess;
}

bool PasTransaction::markExecutionRegister( PasConnectionAgency& refConnection )
{
#ifdef PAS_IGNORE_SET_EXECUTION_REGISTER
    return true;
#else
    TA_Base_Bus::DataBlock<unsigned short> dbData( 
        PAS_ADDRESS_EXECUTION_REGISTER, PAS_ADDRESS_EXECUTION_REGISTER );
    dbData.set( PAS_ADDRESS_EXECUTION_REGISTER, 1 );

    return refConnection.writeToPasWithRetry( dbData );
#endif
}

bool PasTransaction::resetExecutionRegister( PasConnectionAgency& refConnection )
{
    TA_Base_Bus::DataBlock<unsigned short> dbData( 
        PAS_ADDRESS_EXECUTION_REGISTER, PAS_ADDRESS_EXECUTION_REGISTER );
    dbData.set( PAS_ADDRESS_EXECUTION_REGISTER, 0 );

    return refConnection.writeToPasWithRetry( dbData );
}

bool PasTransaction::readCommandResult( PasConnectionAgency& refConnection, unsigned short& usResult )
{
    TA_Base_Bus::DataBlock<unsigned short> dbData( 
        PAS_ADDRESS_COMMAND_RESULT, PAS_ADDRESS_COMMAND_RESULT );

    if ( !refConnection.readFromPasWithRetry( dbData ))
    {
        return false;
    }

    usResult = dbData[PAS_ADDRESS_COMMAND_RESULT];
    return true;
}

}
