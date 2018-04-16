/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/IConfigAtsPowerData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * IConfigAtsPowerData is an interface to an AtsPowerData object. 
  * Its purpose is to give access to information about a given Ats Electrical Subsection.
  *
  */

#if !defined(AFX_ICONFIGATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
#define AFX_ICONFIGATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/IAtsPowerData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{
	class IConfigAtsPowerData : public virtual TA_Base_Core::IConfigItem, public virtual IAtsPowerData
	{	
		
	public:
		virtual ~IConfigAtsPowerData() {};
										
		/**
		 *	setElectricalSubSectionId()
		 *
		 *  Returns the id of the Electrical SubSection within the main line
		 *
		 *  @param		subsection id
		 *
		 */
		virtual void setElectricalSubSectionId(unsigned long id) = 0;
		
		/**
		 *	setLocationId()
		 *
		 *  Returns the location id for this record
		 *
		 *  @param		location id
		 *
		 */
		virtual void setLocationId(unsigned long id) = 0;

		/**
		 *	setTractionPowerDataPointEntityKey()
		 *
		 *  Returns the entity key of the ISCS datapoint which corresponds to the electrical 
		 *  subsection within the main line
		 *
		 *  @param		entity key
		 *
		 */
		virtual void setTractionPowerDataPointEntityKey(unsigned long key) = 0;

		/**
         * deleteThisObject
         *
         * Remove this AtsPowerData from the database. Once this method has been executed, the
         * AtsPowerDataHelper object should be destroyed.
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
         * pre: - this AtsPowerData was initially loaded from the database
         *      - writeAtsPowerDataData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;
	};

	typedef std::vector<IConfigAtsPowerData*> IConfigAtsPowerDataVector;
    typedef IConfigAtsPowerDataVector::iterator  IConfigAtsPowerDataVectorIt;

} // TA_Core

#endif // !defined(AFX_IATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
