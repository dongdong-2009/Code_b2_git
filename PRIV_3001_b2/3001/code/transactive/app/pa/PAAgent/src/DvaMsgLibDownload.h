/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/DvaMsgLibDownload.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef DVAMSGLIBDOWNLOAD_H
#define DVAMSGLIBDOWNLOAD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class IPasSerialDataProcessor;

class DvaMsgLibDownload : public PasTransaction
{
public:
    DvaMsgLibDownload( const bool bAtOcc, IPasSerialDataProcessor& refProcessor );
    virtual ~DvaMsgLibDownload();

private:
    DvaMsgLibDownload();
	DvaMsgLibDownload( const DvaMsgLibDownload& );
	const DvaMsgLibDownload& operator=( const DvaMsgLibDownload& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    IPasSerialDataProcessor& m_refProcessor;
};

} // namespace TA_IRS_App

#endif // DVAMSGLIBDOWNLOAD_H
