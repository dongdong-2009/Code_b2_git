/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/IConfigAtsTWPData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * IConfigAtsTWPData is an interface to an AtsTWPData object.
  *
  */

#if !defined(AFX_ICONFIGATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
#define AFX_ICONFIGATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/IAtsTWPData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{
	class IConfigAtsTWPData : public virtual TA_Base_Core::IConfigItem, public virtual IAtsTWPData 
	{	

	public:
		
		virtual ~IConfigAtsTWPData() {};

		/**
		 *	setTWPDataPointEntityKey()
		 *
		 *  Returns the ISCS data point entity key which corresponds to given ATS TWP
		 *
		 *  @param		entity key of the ISCS data point
		 *
		 */
		virtual void setTWPDataPointEntityKey(unsigned long key) = 0;

		/**
		 *	setTWPDataPointType()
		 *
		 *  Returns the type of the TWP datapoint. Either "Input" or "Output"
		 *
		 *  @param		data point type as string
		 *
		 */
		virtual void setTWPDataPointType(std::string type) = 0;

		/**
		 *	setTWPCommand()
		 *
		 *  Returns the TWP command corresponding to the data point entity
		 *
		 *  @param		TWP command as string
		 *
		 */
		virtual void setTWPCommand(std::string command) = 0;

		/**
         * deleteThisObject
         *
         * Remove this AtsTWPData from the database. Once this method has been executed, the
         * AtsTWPDataHelper object should be destroyed.
         *
         * @exception DatabaseException 
         *            - if there is a problem writing the data to the database.
         *            - if references to this record exists in AT_FIXED_BLOCK_MAP table.  
		 *				These can be removed by specifying the cascade option.
         *
         * @param cascade
         *        - force the removal of this record by removing all references to this record
         *          in other tables (AT_FIXED_BLOCK_MAP).  This should be set to true to prevent the 
         *          DatabaseException from being thrown.
         *
         * pre: - this AtsTWPData was initially loaded from the database
         *      - writeAtsTWPDataData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

	};

} // TA_Core

#endif // !defined(AFX_IATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
