/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/ITelegramReceiver.h $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * This is an interface that receives connect confirmation telegrams.
  * It will be implemented by the application using this protocol library
  */

#if !defined(AFX_ITELEGRAMRECEIVER_H__C2E579A4_F49D_453F_90E0_A09747C54FE2__INCLUDED_)
#define AFX_ITELEGRAMRECEIVER_H__C2E579A4_F49D_453F_90E0_A09747C54FE2__INCLUDED_

#include "boost/shared_ptr.hpp"
#include "bus/signalling/SignallingProtocolLibrary/src/ConnectConfirmationTelegram.h"


class ITelegramReceiver
{
public:
    virtual ~ITelegramReceiver() {};

	/**
	  * receiveConnectConfirmation
	  * 
	  * The telegram to receive and process. 
      * The object pointed to cannot be null and the boost pointer takes care of memory allocation
      * receive a connect confirmation telegram
	  * 
	  * @return virtual void 
	  * @param : boost::shared_ptr<ConnectConfirmationTelegram> connectConfirmation
	  */
    virtual void receiveConnectConfirmation
        (boost::shared_ptr<ConnectConfirmationTelegram> connectConfirmation) = 0;

};

#endif // !defined(AFX_ITELEGRAMRECEIVER_H__C2E579A4_F49D_453F_90E0_A09747C54FE2__INCLUDED_)
