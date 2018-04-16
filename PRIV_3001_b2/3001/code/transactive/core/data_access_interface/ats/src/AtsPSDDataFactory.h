/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsPSDDataFactory.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * AtsPSDDataAccessFactory is a singleton class used to retrieve a data point entity key and type 
  * of a given PSD (Platform Screen Door).  
  */


#if !defined(AFX_ATSPSDDATAFACTORY_H__EBEB534E_EAA1_4B35_9EB8_9211258DBC56__INCLUDED_)
#define AFX_ATSPSDDATAFACTORY_H__EBEB534E_EAA1_4B35_9EB8_9211258DBC56__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ats/src/IAtsPSDData.h"
#include "core/data_access_interface/ats/src/IConfigAtsPSDData.h"

namespace TA_IRS_Core
{
	class AtsPSDDataFactory  
	{		

	private:
		AtsPSDDataFactory() {};
		AtsPSDDataFactory( const AtsPSDDataFactory& AtsPSDDataFactory) {};
		AtsPSDDataFactory& operator=(const AtsPSDDataFactory &) {};

	public:

		/**
		 * getAllPSDData()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing a mapping of a Platform Screen Door (PSD) to an ISCS Data Point.          
         *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDData(const bool readWrite = false);	

		/**
		 * getAllPSDDataInMSSZone()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing a mapping of a Platform Screen Door (PSD) to an ISCS Data Point, found
         * within a given MSSZone
         *
		 * @param		aMSSZone			Zone from inside which PSD mappings should be returned
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataInMSSZone(const std::string &aMSSZone, const bool readWrite = false);					
		
		/**
		 * getAllPSDDataInMSSZoneOfGivenDataPointType()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing a mapping of a Platform Screen Door (PSD) to an ISCS Data Point, found
         * within a given MSSZone and of a given Data Point Type
         *
		 * @param		aMSSZone			Zone from inside which PSD mappings should be returned
		 * @param		aDataPointType		Data Point Type ("Input" / "Output") of interest.
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataInMSSZoneOfGivenDataPointType(const std::string &aMSSZone, const std::string &aDataPointType, const bool readWrite = false);

		/**
		 * getAllPSDDataInMSSZoneOfGivenDataPointNameOfType()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing a mapping of a Platform Screen Door (PSD) to an ISCS Data Point, found
         * within a given MSSZone and of a given Data Point Name of Type
         *
		 * @param		aMSSZone			Zone from inside which PSD mappings should be returned
		 * @param		aDataPointNameOfType	
		 *									Data Point Name of Type ("Door Status" / "Door Inhibit"
		 *									/ "Pre-Close Warning" ) of interest.
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataInMSSZoneOfGivenDataPointNameOfType(const std::string &aMSSZone, const std::string &aDataPointNameOfType, const bool readWrite = false);
		
		/**
		 * getAllPSDDataOfGivenDataPointType()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing a mapping of a Platform Screen Door (PSD) to an ISCS Data Point, 
		 * of a given Data Point Type
         *
		 * @param		aMSSZone			Zone from inside which PSD mappings should be returned
		 * @param		aDataPointType		Data Point Type ("Input" / "Output") of interest.
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataOfGivenDataPointType(const std::string &aDataPointType, const bool readWrite = false);

		/**
		 * getAllPSDDataAtGivenStation()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing a mapping of a Platform Screen Door (PSD) to an ISCS Data Point, 
		 * at a given station
         *
		 * @param		aStationId			Station Id from which PSD mappings are to be fetched		 
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataAtGivenStation(unsigned long aStationId, const bool readWrite = false);		

		/**
		 * getAllPSDDataOfGivenDataPointNameOfType()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing records of a given Data Point Name Of Type
         *
		 * @param		aDataPointNameOfType			
		 *									Name of the data point's type
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataOfGivenDataPointNameOfType(const std::string &aDataPointNameOfType, const bool readWrite = false);
		
		/**
		 * getAllPSDDataAtGivenStationOfGivenDataPointType()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing records of a given Data Point Type at a given Station
         *
		 * @param		aStationId			Id of the station
		 * @param		aDataPointType		Data point's type
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataAtGivenStationOfGivenDataPointType(unsigned long aStationId, const std::string &aDataPointType, const bool readWrite = false);
		
		/**
		 * getAllPSDDataAtGivenStationOfGivenDataPointNameOfType()
         *
         * Fetches records from AT_PSD_ENTITY_MAP table and returns a vector of <IAtsPSDData*> 
		 * each containing records of a given Data Point Type at a given Station
         *
		 * @param		aStationId			Id of the station
		 * @param		aDataPointNameOfType
		 *									Data point's name of type
		 *
         * @exception	DatabaseException	A DatabaseException will be thrown if there is a
         *									problem establishing a connection with the database, or if an
         *									error is encountered while retrieving data.
         * @exception	DataException		A DataException will be thrown if the data cannot be 
         *									converted to the required format (e.g. persist_message should be 
         *									either 0 or 1), or if the wrong amount of data is retrieved.		 
		 */
		std::vector<IAtsPSDData*> getAllPSDDataAtGivenStationOfGivenDataPointNameOfType(unsigned long aStationId, const std::string &aDataPointNameOfType, const bool readWrite = false);
		
		/**
         * createAtsPSDData
         * 
         * Creates an empty IConfigAtsPSDData object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsPSDData object.
         *
         * @return A pointer to an IConfigAtsPSDData object
         *
         */
		IConfigAtsPSDData* createAtsPSDData();

		/**
         * copyAtsPSDData
         * 
         * Creates a new configuration IConfigAtsPSDData object using the IConfigAtsPSDData passed in
         *
         * @param  atsPSDDataToCopy the ConfigAtsBranchTrack to copy
         *
         * @return the new object as a pointer to an IConfigAtsPSDData object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsPSDData* copyAtsPSDData(const IConfigAtsPSDData* atsPSDDataToCopy);

		/**
		 * getInstance()
		 *
		 * Obtains an instance of the class to be used in accessing factory methods.
		 *
		 * @return		AtsPSDDataFactory&	Reference to the instance of this class.
		 *
		 */
		static AtsPSDDataFactory& getInstance();

		virtual ~AtsPSDDataFactory() {};

	};

} // TA_Core

#endif // !defined(AFX_ATSPSDENTITYFACTORY_H__EBEB534E_EAA1_4B35_9EB8_9211258DBC56__INCLUDED_)
