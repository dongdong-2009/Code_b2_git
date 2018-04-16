/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(RadioSessionData_H)
#define RadioSessionData_H

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"

#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{

    /**
     * This class holds cached information about a radio session
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:02 PM
     */
    class RadioSessionData
    {

    public:

        /**
         * This is the radio session corba object
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:02 PM
         */
        typedef TA_Base_Core::NamedObject < TA_Base_Bus::IRadioSessionCorbaDef,
                                            TA_Base_Bus::IRadioSessionCorbaDef_ptr,
                                            TA_Base_Bus::IRadioSessionCorbaDef_var > RadioSessionObject;


        /**
         * Constructor with initial information
         *
         * @param entityData    The entity data to load the radio session information from
         */
        RadioSessionData( TA_Base_Core::RadioSessionEntityDataPtr entityData );


        /**
         * Destructor
         */
        ~RadioSessionData();


        /**
         * Gets the console this radio session is associated with.
         *
         * @return the key if it is associated with a console
         */
        unsigned long getConsoleKey();


        /**
         * Gets the location this radio session is associated with.
         *
         * @return the location key
         */
        unsigned long getLocationKey();


        /**
         * Gets the radio session object to use to call the radio session.
         *
         * @return a reference to the object, as it is cached internally
         */
        RadioSessionObject& getRadioSessionObject();


    private:


        /**
         * Private default constructor
         */
        RadioSessionData();


        /**
         * This is the console key the radio session is associated with
         */
        unsigned long m_consoleKey;


        /**
         * The location of the radio session
         */
        unsigned long m_locationKey;


        /**
         * This is the corba name of the radio session
         */
        TA_Base_Core::CorbaName m_corbaName;


        /**
         * This is the radio session named object. It is created and cached when needed.
         */
        RadioSessionObject* m_agentObject;


        /**
         * Used to lock the radio session object
         */
        TA_Base_Core::NonReEntrantThreadLockable m_agentObjectLock;

    };

} // TA_IRS_App

#endif // !defined(RadioSessionData_H)
