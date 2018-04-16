/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_TIP/TA_BASE/transactive/core/data_access_interface/user_settings/src/IUserSettings.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  * 
  * This interface contains the database retrieval and write methods for the UserSettings library.
  */

#if !defined(IPidSettings_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
#define IPidSettings_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_

#include <string>
#include <vector>
#include "PidMessage.h"


namespace TA_IRS_Core
{
	typedef std::vector<PidMessage>       PidMessages;
	class IPidSettings
	{

	public:

		virtual ~IPidSettings() { };
		virtual PidMessages& getPidMessages() = 0;      
		virtual void setSetting(const int id, const std::wstring& name, const std::wstring& value) =0;

	  
		virtual void addSetting(const int id, const std::wstring& name, const std::wstring& value) = 0;
	};
}

#endif // !defined(IUserSettings_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
