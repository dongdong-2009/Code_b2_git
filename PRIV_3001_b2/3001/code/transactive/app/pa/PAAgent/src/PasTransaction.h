/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasTransaction.h $
 * @author  HuangQi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef PASTRANSACTION_H
#define PASTRANSACTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ace/RW_Mutex.h"

namespace TA_Base_Bus
{
    template<class DataType> class DataBlock;
}

namespace TA_IRS_App
{

class ITransactionOwner;
class PasConnectionAgency;

const unsigned short PAS_COMMAND_SUCCESS = 0; 

class PasTransaction
{
public:
    PasTransaction( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~PasTransaction();

    int execute( PasConnectionAgency& refConnection );
    bool cancelOwnerIfRelated( ITransactionOwner& refOwner );

private:
    PasTransaction();
	PasTransaction( const PasTransaction& );
	const PasTransaction& operator=( const PasTransaction& );

    virtual int executeOccImp( PasConnectionAgency& refConnection ) = 0;
    virtual int executeStnImp( PasConnectionAgency& refConnection ) = 0;

protected:
    bool waitExecutionAcknowledged( PasConnectionAgency& refConnection, const unsigned long ulMilSecTimeout );
    bool markExecutionRegister( PasConnectionAgency& refConnection );
    bool resetExecutionRegister( PasConnectionAgency& refConnection );
    bool readCommandResult( PasConnectionAgency& refConnection, unsigned short& usResult );

private:
    ITransactionOwner* m_pOwner;
    const bool m_bNoOwner;
    const bool m_bAtOcc;

    mutable ACE_RW_Mutex m_mtxOwnerLock;
};

} // namespace TA_IRS_App

#endif // PASTRANSACTION_H
