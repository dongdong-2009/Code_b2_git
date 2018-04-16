/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/ITransactionExecutor.h $
 * @author  J. Dalin
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef ITRANSACTIONEXECUTOR_H
#define ITRANSACTIONEXECUTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
class PasTransaction;

class ITransactionExecutor
{
public:
    virtual int executeTransaction( PasTransaction& refTransaction, const bool bReadonly ) = 0;
};

} // namespace TA_IRS_App

#endif // ITRANSACTIONEXECUTOR_H
