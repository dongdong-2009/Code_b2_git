/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StopBackgroundMusic.h $
 * @author  J. Dalin
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef StopBackgroundMusic_H
#define StopBackgroundMusic_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class StopBackgroundMusic : public PasTransaction
{
public:
    StopBackgroundMusic( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~StopBackgroundMusic();

    void setSourceNumber(const unsigned short usSourceNumber);

private:
    StopBackgroundMusic();
	StopBackgroundMusic( const StopBackgroundMusic& );
	const StopBackgroundMusic& operator=( const StopBackgroundMusic& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    unsigned short m_usSourceNumber;
};

} // namespace TA_IRS_App

#endif // StopBackgroundMusic_H
