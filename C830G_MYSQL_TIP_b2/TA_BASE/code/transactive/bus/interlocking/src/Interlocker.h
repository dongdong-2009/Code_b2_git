#ifndef Bus_Layer_Interlocker_H
#define Bus_Layer_Interlocker_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/Interlocker.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
#include "bus/interlocking/src/RuleContainer.h"

namespace TA_Base_Bus
{
    class InterlockingRule;
    class IBooleanContext;
    class IRealNumberContext;
    class IStringContext;

    /**
     * Interlocker allows the client to add contexts and determine if an state change is allowed.
     */
    class Interlocker
    {
    public:

        /**
          * Constructor
          */
        Interlocker();

        /**
          * Constructor
          */
        Interlocker( const unsigned long p_entityKey, const std::string& p_entityType );

        /**
          * Destructor
          */
        virtual ~Interlocker();

        /**
          * permitStateChange
          *
          * @param The entity key of the device whos state we want to change
          * @param The state we want to change to (this can be the string representation of
          *        a number, a boolean constant (true or false) or a variable name that can be
          *        looked up within the supplied context).
          * @param If this is a launch/precondition check(true) or a return/postcondition check(false).
          *
          * @return TRUE if the state change is allowed, FALSE otherwise
          */
        bool permitStateChange( unsigned long p_entityKey, const std::string& p_desiredState, const bool preCondition = true);

        /**
          * permitStateChange
          *
          * @param The entity key of the device whos state we want to change
          * @param The action we want to preform on this entity
          * @param The action parameter.
          *        eg1 if entity is a sign the action could be "set" and the parameter could be "scene 3"
          *        eg2 if entity is a setpoint the action could be "set" and the parameter could be "50%"
          * @param If this is a launch/precondition check(true) or a return/postcondition check(false).
          *
          * @return TRUE if the state change is allowed, FALSE otherwise
          */
        bool permitStateChange( unsigned long p_entityKey, const std::string& p_action, const std::string& p_parameter, const bool preCondition = true);

        /**
          * addBooleanContext
          *
          * This method is used to add boolean contexts to the interlocker.
          * These contexts will be used to retrieve the values for any boolean variables
          * included in supplied expressions.
          *
          * NOTE - The rule container does not take ownership of these context objects. That
          *        is, it will NOT delete them. However, they do need to exist for the lifetime
          *        of the rule container. You should delete them after deleting the rule
          *        container object.
          *
          * @param The context to add
          */
        void addBooleanContext( const IBooleanContext* p_booleanContext );

        /**
          * addRealNumberContext
          *
          * This method is used to add real number contexts to the interlocker.
          * These contexts will be used to retrieve the values for any real number variables
          * included in supplied expressions.
          *
          * NOTE - The rule container does not take ownership of these context objects. That
          *        is, it will NOT delete them. However, they do need to exist for the lifetime
          *        of the rule container. You should delete them after deleting the rule
          *        container object.
          *
          * @param The context to add
          */
        void addRealNumberContext( const IRealNumberContext* p_realNumContext );

        /**
          * addStringContext
          *
          * This method is used to add string contexts to the interlocker.
          * These contexts will be used to retrieve the values for any string variables
          * included in supplied expressions.
          *
          * NOTE - The rule container does not take ownership of these context objects. That
          *        is, it will NOT delete them. However, they do need to exist for the lifetime
          *        of the rule container. You should delete them after deleting the rule
          *        container object.
          *
          * @param The context to add
          */
        void addStringContext( const IStringContext* p_stringContext );

    private:

        // Disable copy constructor and assignment operator
        Interlocker( const Interlocker& theInterlocker);
        Interlocker& operator=(const Interlocker &);

        // The rule container
        std::auto_ptr<TA_Base_Bus::RuleContainer> m_ruleContainer;

    };
}
#endif // Bus_Layer_Interlocker_H
