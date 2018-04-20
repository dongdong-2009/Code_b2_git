/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/IInterlockRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(IInterlockRule_H)
#define IInterlockRule_H

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/interlocking/src/InterlockAlias.h"

namespace TA_Base_Core
{
    /**
     *  The public face for client wanting to store or retrieve Interlocking Rules
     */
    class IInterlockRule : public virtual IItem
    {

    public:

        /**
          * getRuleId
          *
          * Returns the unique id for this rule
          *
          * @return The unique id for this rule
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual unsigned long getKey() = 0;

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
        virtual unsigned long getEntityKey() = 0;

        /**
          * getName
          *
          * Returns the name of this rule.
          *
          * @return The name of this rule rule
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getName() = 0;

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
        virtual std::string getEntityType() = 0;

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
        virtual std::string getDesiredState() = 0;
		
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
        virtual std::vector<std::string> getParameters() = 0;
		
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
        virtual std::string getCondition() = 0;

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
        virtual bool getPreCondition() = 0;

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
        virtual std::string getDescription() = 0;

        /**
          * getEnabled
          *
          * Returns whether or not this rule is enabled.
          *
          * @return is this rule currently enabled.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */ 
        virtual bool getEnabled() = 0;

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
        virtual IInterlockAliasVector getAliases() = 0;

        /**
          * setEntityKey
          *
          * Sets the entity key of the equipment subject to the interlocking rule. 
          *
          * @param p_entityKey The entity key.
          */
        virtual void setEntityKey( unsigned long p_entityKey ) = 0;
		
        /**
          * setName
          *
          * Sets the rule name. 
          *
          * @param p_name The rule name.
          */
        virtual void setName( const std::string& p_entityType ) = 0;
		
        /**
          * setEntityType
          *
          * Sets the entity type of the equipment subject to the interlocking rule. 
          *
          * @param p_entityType The entity key.
          */
        virtual void setEntityType( const std::string& p_entityType ) = 0;
		
        /**
          * setDesiredState
          *
          * Sets the state transition that is subject to the interlocking rule. 
          *
          * @param p_desiredState The desired state.
          */
        virtual void setDesiredState( const std::string& p_desiredState ) = 0;

        /**
          * setParameters
          *
          * Sets the state transition parameters that are subject to the interlocking rule. 
          *
          * @param p_parameters The desired state.
          */
        virtual void setParameters( const std::vector<std::string>& p_parameters ) = 0;

        /**
          * setCondition
          *
          * Sets expression that has to evaluate to true for the state transition to be allowed. 
          *
          * @param p_condition The enabling condition.
          */
        virtual void setCondition( const std::string& p_condition ) = 0;

        /**
          * setPreCondition
          *
          * Sets weather or not this rule is a precondition check.
          *
          * @param p_preCondition The rule is a precondition check.
          */
        virtual void setPreCondition( const bool p_preCondition ) = 0;

        /**
          * setDescription
          *
          * Sets description for this rule.
          *
          * @param p_description The rule description.
          */
        virtual void setDescription( const std::string& p_description ) = 0;

        /**
          * setEnabled
          *
          * Sets weather or not this rule is enabled.
          *
          * @param p_enabled The rule is/not enabled.
          */
        virtual void setEnabled( const bool p_enabled ) = 0;

        /**
          * setAliases
          *
          * Sets the alias list for the variables used in the rule. 
          *
          * @param p_parameters The desired state.
          */
        virtual void setAliases( const IInterlockAliasVector& p_aliases ) = 0;

        virtual ~IInterlockRule() {};
    };
} //close namespace TA_Base_Core

#endif // !defined(IInterlockRule_H)
