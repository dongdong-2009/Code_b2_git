#if !defined(ITrain_51780B29_B68B_46e8_9435_419D423EF5AF__INCLUDED_)
#define ITrain_51780B29_B68B_46e8_9435_419D423EF5AF__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/trains/src/Attic/ITrain.h,v $
  * @author:  Adam Radics
  * @version: $Revision: 1.1.2.4 $
  *
  * Last modification: $Date: 2006/08/17 02:08:55 $
  * Last modified by:  $Author: adamr $
  *
  * This is a configured train in the database
  *
  */

#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include <string>

namespace TA_IRS_Core
{
    class ITrain : public virtual TA_Base_Core::IItem,
				   public virtual TA_Base_Core::IConfigItem
    {		
		
    public:
		
        /** 
          * getTrainId
          *
          * Gets the ID of the train.
          *
          *
          * @return 
          */
		virtual unsigned long getTrainId() = 0;


        /** 
          * getPrimaryTsi
          *
          * gets the Primary Tsi of the train
          *
          * @return The primary tsi
          */
		virtual std::string getPrimaryTsi() = 0;


		/** 
          * getSecondaryTsi 
          *
          * gets the Secondary Tsi of the train
          *
          * @return The secondary tsi
          */
		virtual std::string getSecondaryTsi() = 0;
		
        
        /** 
          * setTrainId
          *
          * sets the train ID
          *
          * @param trainId the new train ID
          */
		virtual void setTrainId(unsigned long trainId) = 0;
		
        
        /** 
          * setPrimaryTsi
          *
          * sets the Primary tsi of the train
          *
          * @param Primary Tsi 
          */
		virtual void setPrimaryTsi(const std::string&) = 0;
		
        
        /** 
          * setSecondaryTsi
          *
          * sets the Secondary tsi of the train
          *
          * @param the Secondary Tsi
          */
		virtual void setSecondaryTsi(const std::string&) = 0;

        /** 
          * deleteThisObject
          *
          * Deletes the item from the database
          *
          */
		virtual void deleteThisObject() = 0;

		/**
         * getDateCreated
         *
         * Returns the date created for this Train
         *
         * @return The date created for this Train as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this Train.
         *
         * @return The date modified for this Train as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;
		
        
        /** 
          * ~ITrain
          *
          * destructor
          */
        virtual ~ITrain()
        {
            // dummy implementation
        };
    };
}
#endif // !defined(ITrain_51780B29_B68B_46e8_9435_419D423EF5AF__INCLUDED_)
