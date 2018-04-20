/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */


#if !defined(InterlockRule_H)
#define InterlockRule_H

#include <string>

#include "core/data_access_interface/interlocking/src/IInterlockRule.h"

namespace TA_Base_Core
{
    class InterlockRuleHelper;
    /**
     * InterlockRule is a concrete implementation of IConfigInterlockRule, which is in turn an implementation
     * of IInterlockRule. It is to be used to retrieve read only copies of Interlocking Rules
     */
    class InterlockRule : public IInterlockRule
    {

    public:

        /**
          * Constructor
          */
        InterlockRule(const unsigned long key);


        virtual ~InterlockRule();


    public:
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
          * setEntityKey
          *
          * Sets the entity key of the equipment subject to the interlocking rule. 
          *
          * @param password The entity key.
          */
        virtual void setEntityKey( unsigned long p_entityKey );
		
        /**
          * setName
          *
          * Sets the rule name. 
          *
          * @param p_name The rule name.
          */
        virtual void setName( const std::string& p_entityType );
		
        /**
          * setEntityType
          *
          * Sets the entity type of the equipment subject to the interlocking rule. 
          *
          * @param p_entityType The entity key.
          */
        virtual void setEntityType( const std::string& p_entityType );
		
        /**
          * setDesiredState
          *
          * Sets the state transition that is subject to the interlocking rule. 
          *
          * @param password The desired state.
          */
        virtual void setDesiredState( const std::string& p_desiredState );

        /**
          * setParameters
          *
          * Sets the state transition parameters that are subject to the interlocking rule. 
          *
          * @param p_parameters The desired state.
          */
        virtual void setParameters( const std::vector<std::string>& p_parameters );

        /**
          * setCondition
          *
          * Sets expression that has to evaluate to true for the state transition to be allowed. 
          *
          * @param password The enabling condition.
          */
        virtual void setCondition( const std::string& p_condition );

        /**
          * setPreCondition
          *
          * Sets weather or not this rule is a precondition check.
          *
          * @param p_preCondition The rule is a precondition check.
          */
        virtual void setPreCondition( const bool p_preCondition );

        /**
          * setDescription
          *
          * Sets description for this rule.
          *
          * @param p_description The rule description.
          */
        virtual void setDescription( const std::string& p_description );

        /**
          * setEnabled
          *
          * Sets weather or not this rule is enabled.
          *
          * @param p_enabled The rule is/not enabled.
          */
        virtual void setEnabled( const bool p_enabled );

        /**
          * setAliases
          *
          * Sets the alias list for the variables used in the rule. 
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
          * invalidate
          *
          * Make the data contained by this rule as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        virtual void invalidate();

    private:

        // Assignment operator not used so it is made private
		InterlockRule& operator=(const InterlockRule &);
		
        // Copy constructor should not be used and has therefore been made private. 
        InterlockRule( const InterlockRule& theRule);  

        TA_Base_Core::InterlockRuleHelper * m_interlockRuleHelper;
    };
} // closes TA_Base_Core

#endif // !defined(InterlockRule_H)
