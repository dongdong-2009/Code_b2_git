/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/AbstractIncomingIasMessage.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  AbstractIncomingIasMessage.h
//  Implementation of the Class AbstractIncomingIasMessage
//  Created on:      04-Mar-2005 08:23:56 AM
//  Original author: Sean Liew
///////////////////////////////////////////////////////////

#if !defined(AbstractIncomingIasMessage_EBB94462_859E_4a34_81E7_DA4657E7B491__INCLUDED_)
#define AbstractIncomingIasMessage_EBB94462_859E_4a34_81E7_DA4657E7B491__INCLUDED_

#include "AbstractIasMessage.h"

/**
 * Placeholder for elements specific to incoming IAS Messages.  Used to enforce
 * the distinction between incoming and outgoing IAS messages.
 * 
 * Note that despite its name, through the evolution of the design this is no
 * longer an abstract class.
 * @author Sean Liew
 * @version 1.0
 * @created 04-Mar-2005 08:23:56 AM
 */
class AbstractIncomingIasMessage : public AbstractIasMessage 
{

public:
	AbstractIncomingIasMessage();
	virtual ~AbstractIncomingIasMessage();

};
#endif // !defined(AbstractIncomingIasMessage_EBB94462_859E_4a34_81E7_DA4657E7B491__INCLUDED_)