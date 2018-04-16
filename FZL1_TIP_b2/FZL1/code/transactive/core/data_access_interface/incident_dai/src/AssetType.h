/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/AssetType.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Contains the details pertaining to an asset.
  */
#if !defined(AssetType_80D156C7_5037_4dfb_8167_EC648D6B63D8__INCLUDED_)
#define AssetType_80D156C7_5037_4dfb_8167_EC648D6B63D8__INCLUDED_

/**
 * Contains the details pertaining to an asset.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:33:39 PM
 */
#include <string>

namespace TA_IRS_Core
{
	struct AssetType
	{
		
	public:
	/**
	* The ID of the asset
		*/
		unsigned long id;
		/**
		* The actual description of the asset
		*/
		std::string description;
		/**
		* The name of the subsystem
		*/
		std::string subsystem;
		
	};
} // end TA_IRS_Core
#endif // !defined(AssetType_80D156C7_5037_4dfb_8167_EC648D6B63D8__INCLUDED_)