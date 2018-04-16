#ifndef FTPCONSTANTS_H
#define FTPCONSTANTS_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/ftp/src/FTPConstants.h $
 * @author:  <your name>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 *  Constant values used by the FTP interface
 */

namespace TA_Base_Core
{
	//
	// FTP Command strings
	//
	const char* const FTP_COMMAND_USERNAME		= 	"USER";
	const char* const FTP_COMMAND_PASSWORD		= 	"PASS";
	const char* const FTP_COMMAND_CHANGEDIR		= 	"CWD";
	const char* const FTP_COMMAND_CHANGETYPE	= 	"TYPE";
	const char* const FTP_COMMAND_PASSIVEMODE	= 	"PASV";
	const char* const FTP_COMMAND_RETRIEVEFILE	= 	"RETR";
	const char* const FTP_COMMAND_SENDFILE		= 	"STOR";
	const char* const FTP_COMMAND_ABORT			= 	"ABOR";
	const char* const FTP_COMMAND_LOGOUT		= 	"QUIT";

	// Commands and replies are ended with this sequence
	const char* const TELNET_EOL_SEQUENCE		= 	"\r\n";

	// Size of buffer for receiving FTP replies
	const unsigned int FTP_RECV_BUFFERSIZE		=	256;
}

#endif // FTPCONSTANTS_H
