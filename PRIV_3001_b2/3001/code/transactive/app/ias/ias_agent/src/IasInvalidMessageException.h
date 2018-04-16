/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/IasInvalidMessageException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  IasInvalidMessageException.h
//  Implementation of the Class IasInvalidMessageException
//  Created on:      04-Mar-2005 08:24:09 AM
///////////////////////////////////////////////////////////

#if !defined(IasInvalidMessageException_4E81D62F_EE74_4d73_8DF4_8D12F7860AC3__INCLUDED_)
#define IasInvalidMessageException_4E81D62F_EE74_4d73_8DF4_8D12F7860AC3__INCLUDED_

#include "TransactiveException.h"

/**
 * Basic exception class thrown when input data cannot be interpreted to create an
 * IAS Message.
 * @version 1.0
 * @created 04-Mar-2005 08:24:09 AM
 */
class IasInvalidMessageException : public TransactiveException 
{

public:
	IasInvalidMessageException();
	virtual ~IasInvalidMessageException();

};
#endif // !defined(IasInvalidMessageException_4E81D62F_EE74_4d73_8DF4_8D12F7860AC3__INCLUDED_)