/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsPSDData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * AtsPSDData is an implementation of IAtsPSDData. 
  */

#if !defined(AFX_ATSPSDDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_)
#define AFX_ATSPSDDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_

#include "core/data_access_interface/ats/src/AtsPSDDataHelper.h"
#include "core/data_access_interface/ats/src/IAtsPSDData.h"

namespace TA_IRS_Core
{
	class AtsPSDData : public IAtsPSDData
	{

	public:
        /**
         * AtsPSDData (constructor)
         *
         * Construct an AtsPSDData class based around a key.
         *
         * @param key The key of this AtsPSDData in the database
         */
        AtsPSDData(const unsigned long idKey);

        /**
         * AtsPSDData (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         */
        AtsPSDData(unsigned long row, TA_Base_Core::IData& data);

		virtual ~AtsPSDData();

		/**
		 *	getPSDDataPointEntityKey()
		 *
		 *	Returns the data point entity key as unsigned long
		 *
		 */
		virtual unsigned long getPSDDataPointEntityKey();
		
		/**
		 *	getPSDDataPointType()
		 *
		 *	Returns the data point type as string
		 *
		 */
		virtual std::string getPSDDataPointType();
		
		/**
		 *	getPSDDataPointNameOfType()
		 *
		 *	Returns the name of the data point type as string
		 *
		 */
		virtual std::string getPSDDataPointNameOfType();
		
		/**
		 *	getStationId()
		 *
		 *	Returns the id of the station as unsigned long
		 *
		 */
		virtual unsigned long getStationId();
		
		/**
		 *	getPlatformId()
		 *
		 *	Returns the id of the platform as unsigned long
		 *
		 */
		virtual unsigned long getPlatformId();

		
		/**
		 *	getPlatformName()
		 *
		 *	Returns the name of the platform as string
		 *
		 */
		virtual std::string getPlatformName();


        /** 
          * getTisPlatformId
          *
          * gets the platform ID of this platform for sending
          * platform information to the STIS server.
          *
          * @return a single digit number (1-9)
          */
        virtual unsigned char getTisPlatformId();

		
		/**
		 *	getPSDNumber()
		 *
		 *	Returns the PSD door number as integer
		 *
		 */
		virtual int getPSDNumber();

		/**
		 *	getMSSZone()
		 *
		 *	Returns the name of the MSS Zone as string
		 *
		 */
		virtual std::string getMSSZone();

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
		AtsPSDData()	{ };

	public:
		static const std::string OUTPUT_DOOR_INHIBIT_STATE;
		static const std::string OUTPUT_PRE_CLOSE_WARNING;
		static const std::string INPUT_DOOR_STATUS;
		static const std::string INPUT_TYPE;
		static const std::string OUTPUT_TYPE;

	protected:
        TA_IRS_Core::AtsPSDDataHelper * m_atsPSDDataHelper;

    private:

        // Assignment AtsBranchTrack not used so it is made private
		AtsPSDData& operator=(const AtsPSDData &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the AtsBranchTrackHelper
        // has only been written for ConfigLocation objects and will not copy the AtsBranchTrack helper
        // for a read-only object (ie this one).
        AtsPSDData( const AtsPSDData& theAtsPSDData);  
	};

} // TA_Core

#endif // !defined(AFX_ATSPSDDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_)
