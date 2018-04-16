/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/CODE_4669/transactive/app/ecs/ECSAgent/src/IAliveDataPointAccess.h $
  * @author:	Wen Ching
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime:  $
  * Last modified by:	$Author:  $
  *
  * Description:
  *
  *	DataPoint CORBA interface, used to work with DataPoints
  *
  */

#ifndef IALIVEDATAPOINTACCESS_H
#define IALIVEDATAPOINTACCESS_H

#include "bus/scada/common_library/src/CommonDefs.h"
#include "boost/shared_ptr.hpp"



class IAliveDataPointAccess
{

   public:

	IAliveDataPointAccess() {};

	virtual ~IAliveDataPointAccess() {};

	/**
	 *
	 * WriteDataPointValueWithoutLoggingEvent
	 *
	 * Change the value associated with the point
	 * (audit message NOT to be logged)
	 *
	 */
    virtual void writeDataPointValueWithoutLoggingEvent ( const std::string& sessionId, unsigned long value, unsigned long triggeringEntityKey, const char * triggeringEntityValue ) = 0; //TD12418

};


#endif



