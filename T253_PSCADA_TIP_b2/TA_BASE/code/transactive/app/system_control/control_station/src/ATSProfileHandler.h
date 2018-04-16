/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/control_station/src/ATSProfileHandler.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: Noel $
  * 
  */


#ifndef __ATS_PROFILE_HANDLER_H_INCLUDED__
#define __ATS_PROFILE_HANDLER_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class ATSProfileHandler
{
public:
	
	// Update ATS Profile 
	static void updateProfile();
	static void clearProfile();

private:
	// Constructor
	ATSProfileHandler();

	// Destructor
	~ATSProfileHandler();
};

#endif //__ATS_PROFILE_HANDLER_H_INCLUDED__
