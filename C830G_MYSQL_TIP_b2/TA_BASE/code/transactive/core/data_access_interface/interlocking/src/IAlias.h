/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/IAlias.h $
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

namespace TA_Base_Core
{
    class IInterlockRule : public virtual IItem
    {

    public:

        typedef struct 
        {
            std::string alias;
            std::string variableName;
            std::string variableType;
        }Alias;

        typedef std::vector<Alias> AliasVector;

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
        virtual bool getEnabled() = 0;

        /**
          * setEntityKey
          *
          * Sets the entity key of the equipment subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param p_entityKey The entity key.
          */
        virtual void setEntityKey( unsigned long p_entityKey ) = 0;
		
        /**
          * setName
          *
          * Sets the rule name. 
          * This updates the database straight away.
          *
          * @param p_name The rule name.
          */
        virtual void setName( const std::string& p_entityType ) = 0;
		
        /**
          * setEntityType
          *
          * Sets the entity type of the equipment subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param p_entityType The entity key.
          */
        virtual void setEntityType( const std::string& p_entityType ) = 0;
		
        /**
          * setDesiredState
          *
          * Sets the state transition that is subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param p_desiredState The desired state.
          */
        virtual void setDesiredState( const std::string& p_desiredState ) = 0;

        /**
          * setParameters
          *
          * Sets the state transition parameters that are subject to the interlocking rule. 
          * This updates the database straight away.
          *
          * @param p_parameters The desired state.
          */
        virtual void setParameters( const std::string& p_parameters ) = 0;

        /**
          * setCondition
          *
          * Sets expression that has to evaluate to true for the state transition to be allowed. 
          * This updates the database straight away.
          *
          * @param p_condition The enabling condition.
          */
        virtual void setCondition( const std::string& p_condition ) = 0;

        /**
          * setPreCondition
          *
          * Sets weather or not this rule is a precondition check.
          * This updates the database straight away.
          *
          * @param p_preCondition The rule is a precondition check.
          */
        virtual void setPreCondition( const bool p_preCondition ) = 0;

        /**
          * setDescription
          *
          * Sets description for this rule.
          * This updates the database straight away.
          *
          * @param p_description The rule description.
          */
        virtual void setDescription( const std::string& p_description ) = 0;

        /**
          * setEnabled
          *
          * Sets weather or not this rule is enabled.
          * This updates the database straight away.
          *
          * @param p_enabled The rule is/not enabled.
          */
        virtual void setEnabled( const bool p_enabled ) = 0;

        virtual ~IInterlockRule() {};
    };
} //close namespace TA_Base_Core

#endif // !defined(IInterlockRule_H)
