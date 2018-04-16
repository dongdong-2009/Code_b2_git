/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/IPasConnectionObserver.h $
 * @author  J. Dalin
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef IPASCONNECTIONOBSERVER_H
#define IPASCONNECTIONOBSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{

class IPasConnectionObserver
{
public:
    virtual void connectionUpdated( bool bConnectionAvailable ) = 0;
};

} // namespace TA_IRS_App

#endif // IPASCONNECTIONOBSERVER_H
