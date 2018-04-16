/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasReading.h $
 * @author  J. Dalin
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef PASREADING_H
#define PASREADING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class IPasDataProcessor;

class PasReading : public PasTransaction
{
public:
    PasReading( const bool bAtOcc, IPasDataProcessor& refProcessor, const int nDataStartAddress, const int nDataEndAddress );
    virtual ~PasReading();

private:
    PasReading();
	PasReading( const PasReading& );
	const PasReading& operator=( const PasReading& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );
    virtual int executeImp( PasConnectionAgency& refConnection );

private:
    IPasDataProcessor& m_refProcessor;
    int m_nDataStartAddress;
    int m_nDataEndAddress;
};

} // namespace TA_IRS_App

#endif // PASREADING_H
