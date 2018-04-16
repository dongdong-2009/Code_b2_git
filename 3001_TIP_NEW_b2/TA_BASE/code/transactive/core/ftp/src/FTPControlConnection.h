#ifndef FTPCONTROLCONNECTION_H
#define FTPCONTROLCONNECTION_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/ftp/src/FTPControlConnection.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Class representing an FTP control connection, by
 * which commands/command responses are passed between
 * the FTP client/server.
 *
 */

#include "core/sockets/src/TcpSocket.h"

namespace TA_Base_Core
{
	class FTPControlConnection : public TA_Base_Core::TcpSocket
	{

	public:

		/**
		 * Constructor
		 *
		 * FTPControlConnection does not currently add any new functionality to
		 * the TcpSocket class, but may do in future, to aid the addition of
		 * new features.
		 *
		 */
		FTPControlConnection( const std::string& hostAddress, const std::string& hostPort )
		: TA_Base_Core::TcpSocket( hostAddress, hostPort ) {}


		/**
		 * Destructor
		 *
		 */
		virtual ~FTPControlConnection() {}

	};
}


#endif // FTPCONTROLCONNECTION_H
