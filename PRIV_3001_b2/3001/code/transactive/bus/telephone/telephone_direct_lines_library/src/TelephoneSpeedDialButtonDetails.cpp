/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_direct_lines_library/src/TelephoneSpeedDialButtonDetails.cpp $
  * @author:  Catherine Loh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class provides the managed process interface for the application.  This
  * handles the creation of the Telephone Speed Dial Button Details class and  the
  * monitoring of changes passed from the control station.
  */

#include "bus/telephone/telephone_direct_lines_library/src/TelephoneSpeedDialButtonDetails.h"
#include "core/utilities/src/DebugUtil.h"

TelephoneSpeedDialButtonDetails::TelephoneSpeedDialButtonDetails()
 : buttonPosition(0), label(""), SetName(""), telephoneNumber("")
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
	
}

TelephoneSpeedDialButtonDetails::TelephoneSpeedDialButtonDetails(const TelephoneSpeedDialButtonDetails &copy)
{

    FUNCTION_ENTRY("TelephoneSpeedDialButtonDetails");
    
    buttonPosition = copy.buttonPosition;
    label = copy.label;
    SetName = copy.SetName;
    telephoneNumber = copy.telephoneNumber;

	FUNCTION_EXIT;
}

TelephoneSpeedDialButtonDetails& TelephoneSpeedDialButtonDetails::operator=(const TelephoneSpeedDialButtonDetails &copy)
{

    FUNCTION_ENTRY("operator=");
    
    buttonPosition = copy.buttonPosition;
    label = copy.label;
    SetName = copy.SetName;
    telephoneNumber = copy.telephoneNumber;

	FUNCTION_EXIT;
	return *this;
}
