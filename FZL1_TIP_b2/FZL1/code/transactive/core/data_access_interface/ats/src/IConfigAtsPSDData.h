/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/IConfigAtsPSDData.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IConfigAtsPSDData - This is the interface supported by objects that allow configuration of
  * AtsPSDData. It provides methods for retrieving, settings, and writing data for an AtsPSDData object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigAtsPSDData_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigAtsPSDData_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/IAtsPSDData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigAtsPSDData : public virtual TA_Base_Core::IConfigItem, public virtual IAtsPSDData
    {

    public:

        
        virtual ~IConfigAtsPSDData() {};

		/**
		 *	setPSDDataPointEntityKey()
		 *
		 *  Sets the ISCS data point entity key which corresponds to given Ats PSD door number
		 *
		 *  @param		entity key of the ISCS data point
		 *
		 */
		virtual void setPSDDataPointEntityKey(unsigned long key) = 0;

		/**
		 *	setPSDDataPointType()
		 *
		 *  Sets the ISCS data point's type.
		 *
		 *  @param		Type as string. One of "Input" / "Output"
		 *
		 */
		virtual void setPSDDataPointType(const std::string& type) = 0;

		/**
		 *	getPSDDataPointNameOfType()
		 *
		 *  Sets the ISCS data point type's name.
		 *
		 *  @param		Name as string. One of "Door Status" / "Door Inhibit" / "Pre-Close Warning"
		 *
		 */
		virtual void setPSDDataPointNameOfType(const std::string& nameOfType) = 0;
		
		/**
		 *	setPlatformId()
		 *
		 *  Sets the Id of the platform where this PSD is found
		 *
		 *  @param		platform Id
		 *
		 */
		virtual void setPlatformKey(unsigned long key) = 0;
		
		/**
		 *	setPSDNumber()
		 *
		 *  Sets the number of the PSD door
		 *
		 *  @param		PSD Number
		 *
		 */
		virtual void setPSDNumber(int psdNumber) = 0;

        /**
         * deleteThisObject
         *
         * Remove this AtsPSDData from the database. Once this method has been executed, the
         * AtsPSDDataHelper object should be destroyed.
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
         * pre: - this AtsPSDData was initially loaded from the database
         *      - writeAtsPSDDataData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
    
    typedef std::vector<IConfigAtsPSDData*> IConfigAtsPSDDataVector;
    typedef IConfigAtsPSDDataVector::iterator    IConfigAtsPSDDataVectorIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigAtsPSDData_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
