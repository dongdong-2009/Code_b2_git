/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AcknowledgeFireAlarm.h $
 * @author  HuangQi
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef ACKNOWLEDGEFIREALARM_H
#define ACKNOWLEDGEFIREALARM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class AcknowledgeFireAlarm : public PasTransaction
{
public:
    AcknowledgeFireAlarm( const bool bAtOcc );
    virtual ~AcknowledgeFireAlarm();

    void setStationId( const unsigned short usStationId );

private:
	AcknowledgeFireAlarm( const AcknowledgeFireAlarm& );
	const AcknowledgeFireAlarm& operator=( const AcknowledgeFireAlarm& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    unsigned short m_usStationId;
};

} // namespace TA_IRS_App

#endif // ACKNOWLEDGEFIREALARM_H
