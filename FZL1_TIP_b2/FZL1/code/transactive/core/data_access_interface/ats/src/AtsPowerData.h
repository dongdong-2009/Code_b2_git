/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsPowerData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * AtsPowerData is an implementation of IAtsPowerData. 
  */

#if !defined(AFX_ATSPOWERDATA_H__24959B4D_99B2_4C66_8349_6776BBBAF172__INCLUDED_)
#define AFX_ATSPOWERDATA_H__24959B4D_99B2_4C66_8349_6776BBBAF172__INCLUDED_

#include "core/data_access_interface/ats/src/AtsPowerDataHelper.h"
#include "core/data_access_interface/ats/src/IAtsPowerData.h"

namespace TA_IRS_Core
{
	class AtsPowerData : public IAtsPowerData
	{

	public:	
		/**
         * AtsPowerData (constructor)
         *
         * Construct an AtsPowerData class based around a key.
         *
         * @param key The key of this AtsPowerData in the database
         */
        AtsPowerData(const unsigned long idKey);

        /**
         * AtsPowerData (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         */
        AtsPowerData(unsigned long row, TA_Base_Core::IData& data);

		virtual ~AtsPowerData();

		/**
		 *	getTractionPowerDataPointEntityKey()
		 *
		 *  Returns the ISCS entity key of the data point which corresponds directly to
		 *  the ATS Electrical subsection. Based on this key, status of the Electrical
		 *  subsection can be retrieved
		 *
		 */
		unsigned long getTractionPowerDataPointEntityKey();

		/**
		 *	getTractionPowerBitPosition()
		 *
		 *  Returns the bit position of the data point which corresponds directly to
		 *  the ATS Electrical subsection. Based on this key, status of the Electrical
		 *  subsection can be retrieved
		 *
		 */
		unsigned long getTractionPowerBitPosition();
		
		/**
		 *	getElectricalSubSectionId()
		 *
		 *  Returns the Id of the ATS Electrical Sub Section
		 *
		 */
		unsigned long getElectricalSubSectionId();

		/**
		 *	getLocationId()
		 *
		 *  Returns the Id of the location at which this ATS Electrical Sub Section can be found
		 *
		 */
		unsigned long getLocationId();

		/**
		 *	getMSSZone()
		 *
		 *  Returns the name of the MSS Zone within which this ATS Electrical Sub Section is found
		 *
		 */
		std::string getMSSZone();

		/**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();
		

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

	
    private:

        // Assignment AtsBranchTrack not used so it is made private
		AtsPowerData& operator=(const AtsPowerData &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the AtsBranchTrackHelper
        // has only been written for ConfigLocation objects and will not copy the AtsBranchTrack helper
        // for a read-only object (ie this one).
        AtsPowerData( const AtsPowerData& theAtsPowerData); 

	protected:
        TA_IRS_Core::AtsPowerDataHelper * m_atsPowerDataHelper; 

	};

} // TA_Core

#endif // !defined(AFX_ATSPOWERDATA_H__24959B4D_99B2_4C66_8349_6776BBBAF172__INCLUDED_)
