/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ITransactionOwner.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef ITRANSACTIONOWNER_H
#define ITRANSACTIONOWNER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{

class ITransactionOwner
{
public:
    virtual void transactionExecuted( int nResult ) = 0;
};

} // namespace TA_IRS_App

#endif // ITRANSACTIONOWNER_H
