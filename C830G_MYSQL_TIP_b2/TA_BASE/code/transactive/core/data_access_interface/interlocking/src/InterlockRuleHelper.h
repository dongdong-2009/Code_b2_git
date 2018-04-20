/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockRuleHelper.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(InterlockRuleHelper_H)
#define InterlockRuleHelper_H

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/interlocking/src/InterlockAlias.h"

namespace TA_Base_Core
{
    /**
     * Helper class that does all nitty gritty stuff
     */
    class InterlockRuleHelper
    {

    public:
        /**
         * InterlockRuleHelper
         * 
         * This constructor creates a new InterlockRuleHelper for the specified key.
         *
         * @param key The key into the LK_INTERLOCK table for this rule
         */
        InterlockRuleHelper(const unsigned long key);


        /**
         * InterlockRuleHelper
         * 
         * Copy Constructor
         */
        InterlockRuleHelper( const InterlockRuleHelper& theInterlockRuleHelper);


        /**
         * InterlockRuleHelper
         * 
         * This constructor creates a new InterlockRuleHelper for configuration, initiailising
         * the key to 0 - it will be specified once this rule is written to the database
         * for the first time.
         */
        InterlockRuleHelper();


        virtual ~InterlockRuleHelper();

    public:

        /**
         * isNew
         *
         * This returns whether this is a new rule or not
         *
         * @return bool - True if this is a new rule
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        
        /**
         * isValid
         *
         * This returns whether data is valid or not
         *
         * @return bool - True if data is valid
         */
        virtual bool isValid() const
        {
            return m_isValidData;
        };

        
        /**
         * getKey
         *
         * Returns the key for this Interlocking Rule.
         *
         * @return The key for this rule as an unsigned long.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockRuleData() has already been called
         */
        unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this rule. If this is the first time data for this rule
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - setName() has already been called
         */
        std::string getName();


        /**
         * setName
         *
         * Sets the name of this rule. This should only be used by the ConfigInterlockRule class
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount of data is retrieved.
         *
         * @param name The name to give this rule.
         */
        void setName(const std::string& name);
        
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
        unsigned long getEntityKey();

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
        std::string getEntityType();

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
        std::string getDesiredState();
		
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
        std::vector<std::string> getParameters();
		
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
        std::string getCondition();

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
        bool getPreCondition();

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
        std::string getDescription();

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
        bool getEnabled();

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
        IInterlockAliasVector getAliases();

       /**
          * setEntityKey
          *
          * Sets the entity key of the equipment subject to the interlocking rule. 
          *
          * @param password The entity key.
          */
        void setEntityKey( unsigned long p_entityKey );
		
        /**
          * setEntityType
          *
          * Sets the entity type of the equipment subject to the interlocking rule. 
          *
          * @param p_entityType The entity key.
          */
        void setEntityType( const std::string& p_entityType );
		
        /**
          * setDesiredState
          *
          * Sets the state transition that is subject to the interlocking rule. 
          *
          * @param password The desired state.
          */
        void setDesiredState( const std::string& p_desiredState );

        /**
          * setParameters
          *
          * Sets the state transition parameters that are subject to the interlocking rule. 
          *
          * @param p_parameters The desired state.
          */
        void setParameters( const std::vector<std::string>& p_parameters );

        /**
          * setCondition
          *
          * Sets expression that has to evaluate to true for the state transition to be allowed. 
          *
          * @param password The enabling condition.
          */
        void setCondition( const std::string& p_condition );

        /**
          * setPreCondition
          *
          * Sets weather or not this rule is a precondition check.
          *
          * @param p_preCondition The rule is a precondition check.
          */
        void setPreCondition( const bool p_preCondition );

        /**
          * setDescription
          *
          * Sets description for this rule.
          *
          * @param p_description The rule description.
          */
        void setDescription( const std::string& p_description );

        /**
          * setEnabled
          *
          * Sets weather or not this rule is enabled.
          *
          * @param p_enabled The rule is/not enabled.
          */
        void setEnabled( const bool p_enabled );

        /**
          * setAliases
          *
          * Sets the list of aliases for the variables used by this rule
          *
          * @param p_aliases The list of variable aliases for this rule.
          */
        void setAliases( const IInterlockAliasVector& p_aliases );

        /**
         * invalidate
         *
         * Make the data contained by this rule as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         * It is a precondition that writeInterlockRuleData() has been called prior to calling
         * this method on a new rule, as it does not make any sense to invalidate an
         * rule that has not yet been written to the database.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockRuleData() has already been called
         */
        void invalidate();


        /**
         * writeInterlockRuleData
         * 
         * Write this rule to the database. If this is a new rule, this will populate
         * the m_key field (which can be retrieved using getKey()).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the InterlockRule object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeInterlockRuleData();

        /**
         * deleteInterlockRule
         *
         * Remove this rule from the database. Once this method has been executed, the
         * InterlockRuleHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockRuleData() has already been called
         */
        void deleteInterlockRule();
		

    private:
        // Made private as it is not used
		InterlockRuleHelper& operator=(const InterlockRuleHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockRuleData() has already been called
         */
        void reload();


        /**
         * reloadAliases()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockRuleData() has already been called
         */
        void reloadAliases();

        
        /**
         * modifyExistingInterlockRule
         *
         * This will update an existing rule in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the rule object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingInterlockRule();

        
        /**
         * addNewInterlockRule
         *
         * This will add a new rule in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the rule object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewInterlockRule();


        /**
         * getParametersAsString
         *
         * This is a helper function used to stringify a vector for storage in the database
         */
        std::string getParametersAsString(IDatabase& databaseConnection);


        /**
         * getParametersAsVector
         *
         * This is a helper function used to vectorize the parameters after retriaval from the db
         */
        std::vector<std::string> getParametersAsVector(const std::string& parametersString);

        // This is the maximum number of rules allowed
        static const unsigned int MAX_RULES;

        // This is the string to append to a name when it is being deleted
        static const std::string DELETED_FLAG;

        // These are used to store booleans in the db
        static const std::string STRING_TRUE;
        static const std::string STRING_FALSE;

        // This is used to spilt up the action parameters
        static const std::string PARAMETER_SEPARATOR;

        unsigned long m_key;
        std::string m_name;
        std::string m_description;

        unsigned long m_entityKey;
        std::string m_entityType;
        std::string m_desiredState;
        std::vector<std::string> m_parameters;
        std::string m_condition;
        bool m_preCondition;
        bool m_enabled;
        IInterlockAliasVector m_aliases;

        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Base_Core

#endif // !defined(InterlockRuleHelper_H)
