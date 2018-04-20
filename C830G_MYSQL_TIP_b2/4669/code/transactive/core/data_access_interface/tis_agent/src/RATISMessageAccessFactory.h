#ifndef RATIS_MESSAGE_ACCESS_FACTORY_H_INCLUDED
#define RATIS_MESSAGE_ACCESS_FACTORY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/RATISMessageAccessFactory.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * RATISMessageAccessFactory is a singleton that is used to generate and retrieve 
  * IRATISMessage objects.
  */

#include <string>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/tis_agent/src/IRATISMessage.h"

/*
namespace TA_IRS_Core
{
   class IDisplayLog;    // Forward declaration.
} */

namespace TA_IRS_Core
{
    
    class RATISMessageAccessFactory
    {
    public:

        /**
         * ~RATISMessageAccessFactory
         *
         * Standard destructor.
         */

        virtual ~RATISMessageAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      RATISMessageAccessFactory&
         *              A reference to an instance of a RATISMessageAccessFactory object.
         */

		static RATISMessageAccessFactory& getInstance();


        /**
         * getRATISMessages
         *
         * Retrieves all RATIS Messages in the ti_ratis_message table
         *
         * @return      std::vector<IRATISMessage*>
         *              A vector of pointers to objects conforming to the IRATISMessage interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

		std::vector<IRATISMessage*> getRATISMessages();


        /**
         * getRATISMessages
         *
         * Retrieves all RATIS Messages in the ti_ratis_message table that requires vetting
         *
         * @return      std::vector<IRATISMessage*>
         *              A vector of pointers to objects conforming to the IRATISMessage interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

		std::vector<IRATISMessage*> getVettingRATISMessages();


        /**
         * getRATISMessage
         *
         * Retrieves the RATIS Message in the ti_ratis_message table
         * with the specified id
         *
         * @return      IPredefinedMessageLibrary*
         *              A pointer to an object conforming to the IPredefinedMessageLibrary interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
		IRATISMessage* getRATISMessage( long id );


		IRATISMessage* createNewRATISMessage();

		std::vector<IRATISMessage*> getRATISMessagesWhere( std::string whereSQL  = "" );

    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        RATISMessageAccessFactory() {};
        RATISMessageAccessFactory& operator=(const RATISMessageAccessFactory &) {};
        RATISMessageAccessFactory( const RATISMessageAccessFactory& ) {};

		static RATISMessageAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
    };

} // end namespace TA_IRS_Core

#endif
