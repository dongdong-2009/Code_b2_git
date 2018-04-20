/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsTWPData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * AtsTWPData is an implementation of IAtsTWPData. 
  */

#if !defined(AFX_ATSTWPDATA_H__7001673F_2D0B_4055_8049_44131E355A69__INCLUDED_)
#define AFX_ATSTWPDATA_H__7001673F_2D0B_4055_8049_44131E355A69__INCLUDED_

#include "core/data_access_interface/ats/src/AtsTWPDataHelper.h"
#include "core/data_access_interface/ats/src/IAtsTWPData.h"

namespace TA_IRS_Core
{

	class AtsTWPData : public IAtsTWPData
	{
	
	public:		

		/**
         * AtsTWPData (constructor)
         *
         * Construct an AtsTWPData class based around a key.
         *
         * @param key The key of this AtsTWPData in the database
         */
        AtsTWPData(const unsigned long idKey);

        /**
         * AtsTWPData (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         */
        AtsTWPData(unsigned long row, TA_Base_Core::IData& data);

		virtual ~AtsTWPData();

		unsigned long getTWPDataPointEntityKey();
		std::string   getTWPDataPointType();
		std::string   getTWPCommand();

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
		AtsTWPData& operator=(const AtsTWPData &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the AtsBranchTrackHelper
        // has only been written for ConfigLocation objects and will not copy the AtsBranchTrack helper
        // for a read-only object (ie this one).
        AtsTWPData( const AtsTWPData& theAtsTWPData); 

	protected:
        TA_IRS_Core::AtsTWPDataHelper * m_atsTWPDataHelper; 

	public:
		static const std::string INPUT_TYPE;
		static const std::string OUTPUT_TYPE;

		// Output commands
		static const std::string NO_WASH;
		static const std::string DAILY_RINSE;
		static const std::string DAILY_DETERGENT;
		static const std::string WEEKLY_WASH;
		static const std::string CAB_END_WASH;
		static const std::string STOP_WP;
		static const std::string RESTART_WP;

		// Input commands
		static const std::string WP_STATUS;
		static const std::string NO_WASH_SELECTED;
		static const std::string DAILY_RINSE_SELECTED;
		static const std::string DAILY_DETERGENT_SELECTED;
		static const std::string WEEKLY_WASH_SELECTED;
		static const std::string CAB_END_WASH_COMPLETED;
		static const std::string TRAIN_WASH_CYCLE_COMPLETED;
		static const std::string TRAIN_SPEED_OUT_OF_RANGE;
		static const std::string TRAIN_STALLED_IN_TWP;
		static const std::string TRAIN_INCORRECTLY_POSITIONED;
		static const std::string WASH_CYCLE_START_FAILURE;
		static const std::string PH_CHECK;
		static const std::string MAINS_WATER_LOW;
		static const std::string SECOND_BRUSH_RINSE;
		static const std::string CAB_END_WASH_GANTRY_FAILURE_HOR;
		static const std::string CAB_END_WASH_GANTRY_FAILURE_VER;
		static const std::string CAB_END_WASH_BRUSH_FAILURE;
		static const std::string EMERGENCY_STOP_ACTIVATED;
		static const std::string PNEUMATIC_FAILURE;
		static const std::string BRUSH_ACTUATION_FAILURE;
		static const std::string SIDE_ROOF_BRUSHING_FAILURE;
		static const std::string END_BRUSHING_FAILURE;
		static const std::string AIR_BLOW_FAILURE;
		static const std::string PRE_WET_FAILURE;
		static const std::string RECYCLED_RINSE_FAILURE;
		static const std::string WATER_RECYCLING_FAILURE;
		static const std::string DETERGENT_FAILURE;
		static const std::string DETERGENT_LOW_ALARMS;
		static const std::string AIR_PRESSURE_LOW;
		static const std::string NON_CRITICAL_SUMMARY_ALARM;

	};

} // TA_Core

#endif // !defined(AFX_ATSTWPDATA_H__7001673F_2D0B_4055_8049_44131E355A69__INCLUDED_)
