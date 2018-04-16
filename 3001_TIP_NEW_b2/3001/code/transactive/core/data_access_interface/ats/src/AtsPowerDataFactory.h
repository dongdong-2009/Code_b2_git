/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsPowerDataFactory.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  *
  */
#if !defined(AFX_ATSPOWERDATAFACTORY_H__A0153ACD_5653_468C_9DAD_BF8AB75C67C8__INCLUDED_)
#define AFX_ATSPOWERDATAFACTORY_H__A0153ACD_5653_468C_9DAD_BF8AB75C67C8__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/ats/src/IAtsPowerData.h"
#include "core/data_access_interface/ats/src/IConfigAtsPowerData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_IRS_Core
{
	class AtsPowerDataFactory  
	{
	public:

		/**
		 * getAllPowerData()
         *
         * Fetches records from AT_POWER_ENTITY_MAP table and returns a vector of <IAtsPowerData*> 
		 * each containing a mapping of an Electrical Subsection to an ISCS Traction Power Data Point.          
         *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPowerData*> getAllPowerData(const bool readWrite = false);

		/**
		 * getAllPowerDataBySubsection()
         *
         * Fetches records from AT_POWER_ENTITY_MAP table and returns a map of vectors of <IAtsPowerData*> 
		 * each containing a mappings of Electrical Subsection to ISCS Traction Power Data Points.          
         *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::map<unsigned long, std::vector<IAtsPowerData*> > getAllPowerDataBySubsection(const bool readWrite = false);

		/**
		 * getAllPowerDataAtLocation()
         *
         * Fetches records from AT_POWER_ENTITY_MAP table and returns a vector of <IAtsPowerData*> 
		 * each containing a mapping of an Electrical Subsection to an ISCS Traction Power Data Point.
		 * The set is restricted to subsections found at a given location based on the location key.
         *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPowerData*> getAllPowerDataAtLocation(unsigned long aLocationKey, const bool readWrite = false);

				/**
		 * getAllPowerDataAtLocation()
         *
         * Fetches records from AT_POWER_ENTITY_MAP table and returns a vector of <IAtsPowerData*> 
		 * each containing a mapping of an Electrical Subsection to an ISCS Traction Power Data Point.
		 * The set is restricted to subsections found at a given location based on the location key.
         *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::map<unsigned long, std::vector<IAtsPowerData*> >  getAllPowerDataAtLocationBySubsection(unsigned long aLocationKey, const bool readWrite = false);

		/**
		 * getAllPowerDataAtMSSZone()
         *
         * Fetches records from AT_POWER_ENTITY_MAP table and returns a vector of <IAtsPowerData*> 
		 * each containing a mapping of an Electrical Subsection to an ISCS Traction Power Data Point.
		 * The set is restricted to subsections found at a given MSS Zone.
         *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPowerData*> getAllPowerDataAtMSSZone(const std::string &aMSSZone, const bool readWrite = false);

		/**
		 * getAllPowerDataAtMSSZone()
         *
         * Fetches records from AT_POWER_ENTITY_MAP table and returns a vector of <IAtsPowerData*> 
		 * each containing a mapping of an Electrical Subsection to an ISCS Traction Power Data Point.
		 * The set is restricted to subsections found at a given MSS Zone.
         *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::map<unsigned long, std::vector<IAtsPowerData*> >  getAllPowerDataAtMSSZoneBySubsection(const std::string &aMSSZone, const bool readWrite = false);

		/**
         * createAtsPowerData
         * 
         * Creates an empty IConfigAtsPowerData object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsPowerData object.
         *
         * @return A pointer to an IConfigAtsPowerData object
         *
         */
		IConfigAtsPowerData* createAtsPowerData();

		/**
         * copyAtsPowerData
         * 
         * Creates a new configuration IConfigAtsPowerData object using the IConfigAtsPowerData passed in
         *
         * @param  atsPSDDataToCopy the ConfigAtsBranchTrack to copy
         *
         * @return the new object as a pointer to an IConfigAtsPowerData object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsPowerData* copyAtsPowerData(const IConfigAtsPowerData* atsPSDDataToCopy);

		/**
		 * getInstance()
		 *
		 * Obtains an instance of the class to be used in accessing factory methods.
		 *
		 * @return		AtsPowerDataFactory&	Reference to the instance of this class.
		 *
		 */
		static AtsPowerDataFactory& getInstance();

	private:

		AtsPowerDataFactory() {};
		AtsPowerDataFactory( const AtsPowerDataFactory& AtsPowerDataFactory) {};
		AtsPowerDataFactory& operator=(const AtsPowerDataFactory &) {};

		std::map<unsigned long, std::vector<IAtsPowerData*> > orderPowerData (std::vector<IAtsPowerData*>& powerData);
	
	public:		
		virtual ~AtsPowerDataFactory();

	};

} // TA_Core

#endif // !defined(AFX_ATSPOWERDATAFACTORY_H__A0153ACD_5653_468C_9DAD_BF8AB75C67C8__INCLUDED_)
