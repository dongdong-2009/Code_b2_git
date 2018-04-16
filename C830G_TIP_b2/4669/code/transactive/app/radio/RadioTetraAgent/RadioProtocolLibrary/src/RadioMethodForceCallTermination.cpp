/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodForceCallTermination.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class RadioMethodForceCallTermination
  *
  */

#include "RadioMethodForceCallTermination.h"
#include "RadioProtocol.h"

using namespace TA_IRS_App;

#define FORCECALLTERMINATION_METHOD_LENGTH        4
#define FORCECALLTERMINATION_CALLREF_POSITION     0



/**
* Constructor
* @param sessionRef
* @param callRef the call reference handle
*/
RadioMethodForceCallTermination::RadioMethodForceCallTermination(RadioSessionReference sessionRef, unsigned long callRef)
    :RadioMethodCallReference(FORCECALLTERMINATION_METHOD_LENGTH, METHOD_FORCECALLTERMINATION, sessionRef, callRef)
{
}

