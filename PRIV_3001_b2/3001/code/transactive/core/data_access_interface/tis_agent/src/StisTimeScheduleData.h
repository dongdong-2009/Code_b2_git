
#ifndef STISTIMESCHEDULEDATA_H 
#define STISTIMESCHEDULEDATA_H 

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/tis_agent/src/StisTimeScheduleData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * StisTimeScheduleData contains the get and set methods that do the 
  * actual database work for the StisTimeScheduleData class.
  * It accesses both the ti_train_time_schedule table.
  */

#pragma warning ( disable : 4786 )

#include <map>
#include <string>
#include <climits>
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

namespace TA_IRS_Core
{
    class StisTimeScheduleData
    {
    public:
       /**
         * StisTimeScheduleData
         * 
         * This constructor creates a StisTimeScheduleData for the specified key.
         *
         * @param       unsigned long trainTimeSchedulePKey
         *              The database key for the StisTimeSchedule parent of this helper.
         */
        StisTimeScheduleData( unsigned long scheduleId);


        /**
         * ~StisTimeScheduleData
         *
         * Standard destructor.
         */
        virtual ~StisTimeScheduleData() {};

        /**
         * getKey
         *
         * Returns the StisTimeSchedule key for this StisTimeSchedule entry.
         *
         * Preconditions:   Either - this VideoOutputGroup entry was initially 
         *                           loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *
         * @return      unsigned long
         *              The key for this VideoOutputGroup.
         */
	    unsigned long getKey();
		std::string getStartTime();
		void setStartTime(const std::string& startTime);
		
		std::string getEndTime();
		void setEndTime(const std::string& endTime);

		unsigned long getPriority();
		void setPriority(unsigned long priority);

		TA_Base_Core::EDisplayMode getDisplayMode();
		void setDisplayMode(TA_Base_Core::EDisplayMode displayMode);

		TA_Base_Core::EScrollSpeed getScrollSpeed();
		void setScrollSpeed(TA_Base_Core::EScrollSpeed scrollSpeed);

		unsigned long getRepeatInterval();
		void setRepeatInterval(unsigned long repeatInterval);

		TA_Base_Core::ELcdFontSize getLcdFontSize();
		void setLcdFontSize(TA_Base_Core::ELcdFontSize lcdFontSize);

		TA_Base_Core::ELcdFontColour getLcdFontColour();
		void setLcdFontColour(TA_Base_Core::ELcdFontColour lcdFontColour);

		TA_Base_Core::ELEDFontSize  getLedFontSize();
		void setLedFontSize(TA_Base_Core::ELEDFontSize ledFontSize);

		TA_Base_Core::ELEDColour  getLedFontColour();
		void setLedFontColour(TA_Base_Core::ELEDColour ledFontColour);

		unsigned long getDisplayTime();
		void setDisplayTime(unsigned long displayTime);

		TA_Base_Core::EJustification getJustification();
		void setJustification(TA_Base_Core::EJustification justification);

		std::string getTextContent();
		void setTextContent(const std::string& textContent);


		virtual bool isRecordEmtpy();

        /**
         * invalidate
         *
         * Marks the data contained by this StisTimeSchedule as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    Either - this StisTimeSchedule was initially loaded from the database
         *                  OR     - writeStisTimeSchedule() has already been called, as it does 
         *                           not make any sense to invalidate a StisTimeSchedule that has not 
         *                           yet been written to the database.
         */
	    virtual void invalidate();
        /** 
         * updateExistingStisTimeSchedule
         *
         * update an existing train time schedule in the database
         *
         * @exception TA_Base_Core::DatabaseException A TA_Base_Core::DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void updateExistingStisTimeSchedule();


        virtual bool hasChanged() {return m_hasChanged;}

    private:

        /**
         * StisTimeScheduleData
         * 
         * Copy constructor
         */
        StisTimeScheduleData( const StisTimeScheduleData& timeScheduleHelper);
        //
        // Disable the assignment operator.
        //

		StisTimeScheduleData& operator=( const StisTimeScheduleData& );
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this StisTimeSchedule was initially loaded from the database
         *                  OR     - writeStisTimeScheduleData() has already been called.
         */
	    void reload();



        //
        // Flag to indicate if the data needs reloading.
        //
	    bool m_isValidData;


        //
        // Flag to indicate whether the data has changed
        //
        bool m_hasChanged;
        unsigned long m_key;
		std::string m_startTime;
		std::string m_endTime;
		unsigned long m_priority;
		TA_Base_Core::EDisplayMode m_displayMode;
		TA_Base_Core::EScrollSpeed m_scrollSpeed;
		unsigned long m_repeatInterval;
		TA_Base_Core::ELcdFontSize m_lcdFontSize;
		TA_Base_Core::ELcdFontColour m_lcdFontColour;
		TA_Base_Core::ELEDFontSize m_ledFontSize;
		TA_Base_Core::ELEDColour   m_ledFontColour;
		unsigned long m_displayTime;
		TA_Base_Core::EJustification m_justification;
		std::string m_textContent;
    };

} // closes TA_Base_Core

#endif
