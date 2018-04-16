/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioSubscriber.h $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Class RadioSubscriber is an implementation of the interface IRadioSubscriber.
  * It holds the data specific to a RadioSubscriber entry in the database, 
  * and allows read-only access to that data.
  */

#if !defined(AFX_RADIO_SUBSCRIBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
#define AFX_RADIO_SUBSCRIBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_

#include <string>
#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "core/data_access_interface/radio/src/RadioSubscriberHelper.h"


namespace TA_IRS_Core
{
    class RadioSubscriber : public IRadioSubscriber  
    {
    public:

	    /**
	      * RadioSubscriber
	      * 
	      * Constructs a train object based on the primary key
	      * 
	      * @param : const unsigned long key
	      */
	    RadioSubscriber(const unsigned long key);
        
	    /**
	      * RadioSubscriber
	      * 
	      * Constructs a new RadioSubscriber object using the information provided in IData interface
	      * 
	      * @param : const unsigned long row
	      * @param : TA_Base_Core::IData& data
	      * 
	      * @see RadioSubscriberHelper::RadioSubscriberHelper(const unsigned long, TA_Base_Core::IData&);
	      */
        RadioSubscriber(const unsigned long row, TA_Base_Core::IData& data);

	    /**
	      * ~RadioSubscriber
	      * 
	      * Destructor 
	      */
	    virtual ~RadioSubscriber();

        //
        // IRadioSubscriber methods
        //

	    /**
	      * getResourceType
	      * 
	      * @return char
	      */
	    virtual char getResourceType();

	    /**
	      * getResourceId
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getResourceId();

	    /**
	      * getSSI
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getSSI();

	    /**
	      * getAlias
	      * 
	      * @return std::string
	      */
	    virtual std::string getAlias();

	    /**
	      * getKey
	      * 
	      * Returns the key for this item.
	      * 
	      * @return unsigned long
	      */
        virtual unsigned long getKey();

		virtual std::string getName();
		
	    /**
	      * invalidate
	      * 
	      * Make the data contained by this item as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
	      * 
	      * @return void 
	      */
        virtual void invalidate();

    private:

	    /**
	      * RadioSubscriber
	      * 
	      * Copy constructor (disabled).
          * If a copy constructor is required care should be taken. The copy constructor to 
          * the RadioSubscriberHelper has only been written for ConfigRadioSubscriber objects and will not copy 
          * the RadioSubscriber helper for a read-only object (ie this one).
	      * 
	      * @param : const RadioSubscriber&
	      */
        RadioSubscriber(const RadioSubscriber&);

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
	      * 
	      * @return RadioSubscriber& 
	      * @param : const RadioSubscriber &
	      */
		RadioSubscriber& operator=(const RadioSubscriber&);

    private:

        RadioSubscriberHelper m_radioSubscriberHelper;
    };
}
#endif // !defined(AFX_RADIO_SUBSCRIBER_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
