/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioException.cpp,v $
  * @author:  Adam Radics
  * @version: $Revision: 1.1.2.1 $
  *
  * Last modification: $Date: 2007/04/03 05:35:47 $
  * Last modified by:  $Author: adamr $
  *
  * Parent of all Radio Agent exceptions.
  *
  */

#include "app/cctv/tvss_agent/TCPCommunication/src/TcpException.h"

using TA_Base_Core::TransactiveException;

namespace TA_IRS_App
{

    TcpException::TcpException() throw()
      : TransactiveException()
    {
    }


    TcpException::TcpException(const std::string& msg) throw()
      : TransactiveException(msg)
    {
    }


    TcpException::~TcpException() throw()
    {
    }

}

