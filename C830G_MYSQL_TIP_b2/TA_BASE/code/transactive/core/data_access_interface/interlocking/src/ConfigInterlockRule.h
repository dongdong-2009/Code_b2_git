/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/ConfigInterlockRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  * 
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */


#if !defined(ConfigInterlockRule_H)
#define ConfigInterlockRule_H

#include <string>
#include <set>

#include "core/data_access_interface/interlocking/src/InterlockAlias.h"
#include "core/data_access_interface/interlocking/src/IConfigInterlockRule.h"

namespace TA_Base_Core
{
    class InterlockRuleHelper;

    /**
     * ConfigInterlockRule is a concrete implementation of IConfigInterlockRule, which is in turn an implementation
     * of IInterlockRule. It is to be used by the Configuration Editor, and other applications
     * that wish to change the database content for interlocking rules.
    */
    class ConfigInterlockRule : public IConfigInterlockRule
    {

    public:
        /**
          * Constructor
          */
        ConfigInterlockRule();


        /**
          * Constructor
          */
        ConfigInterlockRule(const unsigned long key);


        /**
          * Constructor
          */
        ConfigInterlockRule( const ConfigInterlockRule& theConfigInterlockRule);

        
        virtual ~ConfigInterlockRule();


    public:

        /**
         * deleteThisObject
         *
         * Removes this rule from the database. 
         * The calling application MUST then delete this rule object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockRuleData() has already been called
         *      This rule has not been deleted
         */
        void deleteThisObject();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the rule object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This rule has not been deleted
         */
        void applyChanges();
        

        /**
          * getKey
          *
          * Returns the key for this Interlock Rule.
          *
          * @return The key for this interlock rule as an unsigned long.
          */
        unsigned long getKey();

        /**
          * getName
          *
          * Returns the name of this rule. If this is the first time data for this rule
          * has been requested, all the data is loaded from the database.
          *
          * @return The name for this rule as a std::string
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getName();

        /**
          * getEntityKey
          *
          * Returns the entity key of the equipment that is subject to the interlocking rule.
          *
          * @return The entity key of the equipment that is subject to the interlocking rule
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual unsigned long getEntityKey();

        /**
          * getEntityType
          *
          * Returns the entity type of the equipment that is subject to the interlocking rule.
          *
          * @return The entity type of the equipment that is subject to the interlocking rule
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getEntityType();

        /**
          * getDesiredState
          *
          * Returns the state transition that the interlocking rule applies to.
          *
          * @return The state transition that the interlocking rule applies to.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual std::string getDesiredState();
		
        /**
          * getParameters
          *
          * Returns the state transition parameters that the interlocking rule applies to.
          *
          * @return The state transition parameters that the interlocking rule applies to.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual std::vector<std::string> getParameters();
		
        /**
          * getCondition
          *
          * Returns the expression that has to evaluate to true for the state transition to be allowed.
          *
          * @return The expression that has to evaluate to true for the state transition to be allowed.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual std::string getCondition();

        /**
          * getPreCondition
          *
          * Returns weather or not this rule is a precondition check.
          *
          * @return true if rule is a precodition check
          *         false if rule is a postcondition check.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual bool getPreCondition();

        /**
          * getDescription
          *
          * Returns the description of this rule.
          *
          * @return The description for this rule.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual std::string getDescription();

        /**
          * getEnabled
          *
          * Returns weather or not this rule is enabled.
          *
          * @return is this rule currently enabled.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual bool getEnabled();

         /**
          * getAliases
          *
          * Returns the alias list
          *
          * @return the list of aliases for this rule
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual IInterlockAliasVector getAliases();

        /**
          * setName
          *
          * Sets the name of the rule
          *
          * @param The name of the rule
          */
        virtual void setName(const std::string& name);

        /**
          * setEntityKey
          *
          * Sets the entity key of the equipment subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param password The entity key.
          */
        virtual void setEntityKey( unsigned long p_entityKey );
		
        /**
          * setEntityType
          *
          * Sets the entity type of the equipment subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param p_entityType The entity key.
          */
        virtual void setEntityType( const std::string& p_entityType );
		
        /**
          * setDesiredState
          *
          * Sets the state transition that is subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param password The desired state.
          */
        virtual void setDesiredState( const std::string& p_desiredState );

        /**
          * setParameters
          *
          * Sets the state transition parameters that are subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param p_parameters The desired state.
          */
        virtual void setParameters( const std::vector<std::string>& p_parameters );

        /**
          * setCondition
          *
          * Sets expression that has to evaluate to true for the state transition to be allowed. 
          * This updates the database straight away.
          *
          * @param password The enabling condition.
          */
        virtual void setCondition( const std::string& p_condition );
        
        /**
          * setPreCondition
          *
          * Sets weather or not this rule is a precondition check.
          * This updates the database straight away.
          *
          * @param p_preCondition The rule is a precondition check.
          */
        virtual void setPreCondition( const bool p_preCondition );

        /**
          * setDescription
          *
          * Sets description for this rule.
          * This updates the database straight away.
          *
          * @param p_description The rule description.
          */
        virtual void setDescription( const std::string& p_description );

        /**
          * setEnabled
          *
          * Sets weather or not this rule is enabled.
          * This updates the database straight away.
          *
          * @param p_enabled The rule is/not enabled.
          */
        virtual void setEnabled( const bool p_enabled );

        /**
          * setAliases
          *
          * Sets the 
          * This updates the database straight away.
          *
          * @param p_parameters The desired state.
          */
        virtual void setAliases( const IInterlockAliasVector& p_aliases );

        /**
          * save
          *
          * Writes this rule to the database updating the record
          * if it exists or creating a new record if it doesn't
          */
        virtual void save();

        /**
         * hasChanged
         *
         * This should be called to determine if any part of the operator has been changed by the user.
         *
         * @return bool - This will return true if the operator does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged();

        /**
         * isNew
         *
         * This should be called to determine if this operator is new. A new operator is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the operator has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this operator as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this rule. 
         *
         * @return unsigned long - The unique identifier for this rule
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return 0;
        };

        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the rule. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all rule changes
         */
        virtual ItemChanges getAllItemChanges();

        private:
        // Assignment operator not used so it is made private
		ConfigInterlockRule& operator=(const ConfigInterlockRule &);
		
        InterlockRuleHelper* m_interlockRuleHelper;

    };
} // closes TA_Base_Core

#endif // !defined(ConfigInterlockRule_H)
