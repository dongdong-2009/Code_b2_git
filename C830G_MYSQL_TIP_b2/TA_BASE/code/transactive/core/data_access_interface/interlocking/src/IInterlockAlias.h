/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/IInterlockAlias.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(IInterlockAlias_H)
#define IInterlockAlias_H

#include <vector>
#include "boost/shared_ptr.hpp"
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IInterlockAlias;

    typedef std::vector<boost::shared_ptr<TA_Base_Core::IInterlockAlias> > IInterlockAliasVector;

    /**
     * IInterlockAlias - This is a helper class for InterlockRule. It handles storing and retrieving lists
     * of aliases for each rule. 
     * It should never be directly called by the client
     */
    class IInterlockAlias 
    {

    public:

        /**
          * getAlias
          *
          * Returns the alias for this variable.
          *
          * @return the alias for this variable.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getAlias() = 0;

        /**
          * getVariableName
          *
          * Returns the name of this variable.
          *
          * @return The name of this variable
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getVariableName() = 0;

        /**
          * getVariableType
          *
          * Returns the variable type.
          *
          * @return The variable type
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getVariableType() = 0;

        /**
          * setRuleId
          *
          * Sets the rule id this alias relates to. 
          * This method can only be called on a new unsaved alias
          *
          * @param p_name The ruleid.
          */
        virtual void setRuleId( const unsigned int p_ruleId ) = 0;
		
        /**
          * setAlias
          *
          * Sets the alias name. 
          * Call save() to update the database.
          *
          * @param p_name The alias.
          */
        virtual void setAlias( const std::string& p_alias ) = 0;
		
        /**
          * setVariableName
          *
          * Sets the variable name. 
          * Call save() to update the database.
          *
          * @param p_variableName The entity key.
          */
        virtual void setVariableName( const std::string& p_variableName ) = 0;
		
        /**
          * setVariableType
          *
          * Sets the variable type. 
          * Call save() to update the database.
          *
          * @param p_variableType The entity key.
          */
        virtual void setVariableType( const std::string& p_variableType ) = 0;
		
        /**
         * save
         * 
         * Write this alias to the database.
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
        virtual void save() = 0;

        /**
         * erase
         *
         * Remove this alias from the database.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this alias was initially loaded from the database
         *      OR     - writeInterlockAliasData() has already been called
         */
        virtual void erase() = 0;

        virtual ~IInterlockAlias() {};
    };


} //close namespace TA_Base_Core

#endif // !defined(IInterlockAlias_H)
