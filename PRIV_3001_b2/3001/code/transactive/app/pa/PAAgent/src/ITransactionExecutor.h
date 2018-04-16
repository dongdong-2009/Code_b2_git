/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ITransactionExecutor.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
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
