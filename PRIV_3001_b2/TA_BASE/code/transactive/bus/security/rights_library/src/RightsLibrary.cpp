/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/rights_library/src/RightsLibrary.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implementation of RightsLibraryFactory.
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "RightsLibraryInternal.h"

namespace TA_Base_Bus
{

	RightsLibraryFactory::RightsLibraryFactory()
	{
	}

	RightsLibrary* RightsLibraryFactory::buildRightsLibrary( ERightsLibraryType rlType/*=DEFAULT_RL*/, EOwner owner /*= DEFAULT*/ ) const
	{
		//xinsong++ check parameter IgnoreChekcRight
		bool ignoreRights = TA_Base_Core::RunParams::getInstance().isSet(RPARAM_RIGHTCHECKING);
		if ( ignoreRights )
		{
			rlType = ALWAYS_TRUE_RL;
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Application will Ignore Rights");
		}

		FUNCTION_ENTRY("buildRightsLibrary");
		switch( rlType )
		{
			case ALWAYS_TRUE_RL:
				return( new RightsLibrary() );
				break;

			case DEFAULT_RL:
				return( new CorbaRightsLibrary() );
				break;

			default:
				return NULL;
				break;
		}
		FUNCTION_EXIT;
	}
}
