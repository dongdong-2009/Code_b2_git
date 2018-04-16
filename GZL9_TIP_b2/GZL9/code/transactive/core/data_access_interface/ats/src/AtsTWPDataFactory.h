/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsTWPDataFactory.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * AtsTWPDataAccessFactory is a singleton class used to retrieve a data point entity key and type 
  * of a given TWP command
  */

#if !defined(AFX_ATSTWPDATAFACTORY_H__A16EC740_6757_4DD1_9858_0453504EA337__INCLUDED_)
#define AFX_ATSTWPDATAFACTORY_H__A16EC740_6757_4DD1_9858_0453504EA337__INCLUDED_

#include <vector>
#include <string>
#include <sstream>

#include "core/data_access_interface/ats/src/IAtsTWPData.h"
#include "core/data_access_interface/ats/src/IConfigAtsTWPData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	class AtsTWPDataFactory  
	{
	public:
		static AtsTWPDataFactory &getInstance();
		virtual ~AtsTWPDataFactory() {};
		
		std::vector<IAtsTWPData*> getAllTWPData(const bool readWrite = false);
		std::vector<IAtsTWPData*> getAllTWPDataOfDataPointType(const std::string& aDataPointType, const bool readWrite = false);

		/**
         * createAtsTWPData
         * 
         * Creates an empty IConfigAtsTWPData object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsTWPData object.
         *
         * @return A pointer to an IConfigAtsTWPData object
         *
         */
		IConfigAtsTWPData* createAtsTWPData();

		/**
         * copyAtsTWPData
         * 
         * Creates a new configuration IConfigAtsTWPData object using the IConfigAtsTWPData passed in
         *
         * @param  atsPSDDataToCopy the ConfigAtsBranchTrack to copy
         *
         * @return the new object as a pointer to an IConfigAtsTWPData object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsTWPData* copyAtsTWPData(const IConfigAtsTWPData* atsTWPDataToCopy);

	private:
		AtsTWPDataFactory() {};
		AtsTWPDataFactory( const AtsTWPDataFactory& AtsTWPDataFactory) {};
		AtsTWPDataFactory& operator=(const AtsTWPDataFactory &) {};

		

	};

} // TA_Core

#endif // !defined(AFX_ATSTWPDATAFACTORY_H__A16EC740_6757_4DD1_9858_0453504EA337__INCLUDED_)
