/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/PasModbusConnection.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef PASMODBUSCONNECTION_H
#define PASMODBUSCONNECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/modbus_comms/src/SteModbusTcpConnection.h"

namespace TA_IRS_App
{

typedef TA_Base_Bus::ModbusTcpConnection BusModbusTcpConnection;

class PasModbusConnection : public BusModbusTcpConnection
{
public:
    PasModbusConnection( const std::string& host, const std::string& service );
    virtual ~PasModbusConnection();

    bool connect();
    virtual void disable();
    bool isAlive();

    bool readRegisterWithRetry( TA_Base_Bus::DataBlock<unsigned short>& refData, const int nTryTimes );
    bool writeRegisterWithRetry( const TA_Base_Bus::DataBlock<unsigned short>& refData, const int nTryTimes );

private:
    PasModbusConnection();
	PasModbusConnection( const PasModbusConnection& );
	const PasModbusConnection& operator=( const PasModbusConnection& );
};

} // namespace TA_IRS_App

#endif // PASMODBUSCONNECTION_H
