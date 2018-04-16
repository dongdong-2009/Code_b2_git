/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockAlias.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */


#if !defined(InterlockAlias_H)
#define InterlockAlias_H

#include <string>

#include "core/data_access_interface/interlocking/src/IInterlockAlias.h"

namespace TA_Base_Core
{
    /**
     * The concreate implementation of IInterlockAlias.
     * It really does the work.
     */
    class InterlockAlias : public IInterlockAlias
    {

    public:

        /**
          * Constructor
          */
        InterlockAlias();
        InterlockAlias(const unsigned long key);


        virtual ~InterlockAlias();


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
        std::string getAlias();

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
        std::string getVariableName();

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
        std::string getVariableType();

        /**
          * setRuleId
          *
          * Sets the rule id this alias relates to. 
          * This method can only be called on a new unsaved alais
          *
          * @param p_name The alias.
          */
        void setRuleId( const unsigned int ruleId );
		
        /**
          * setAlias
          *
          * Sets the alias name. 
          * Call save() to update the database
          *
          * @param p_name The alias.
          */
        void setAlias( const std::string& p_alias );
		
        /**
          * setVariableName
          *
          * Sets the variable name. 
          * Call save() to update the database
          *
          * @param p_variableName The entity key.
          */
        void setVariableName( const std::string& p_variableName );
		
        /**
          * setVariableType
          *
          * Sets the variable type. 
          * Call save() to update the database
          *
          * @param p_variableType The entity key.
          */
        void setVariableType( const std::string& p_variableType );

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
        void save();

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
        void erase();
		

    private:

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this alias was initially loaded from the database
         *      OR     - writeInterlockAliasData() has already been called
         */
        void reload();


        /**
         * modifyExistingInterlockAlias
         *
         * This will update an existing alias in the database with the settings currently saved here
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
        void modifyExistingInterlockAlias();

        
        /**
         * addNewInterlockAlias
         *
         * This will add a new Alias in the database with the settings currently saved here
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
        void addNewInterlockAlias();

        int m_ruleId;
        std::string m_alias;
        std::string m_variableName;
        std::string m_variableType;

        bool m_isValidData;
        bool m_isNew;
    };


} // closes TA_Base_Core

#endif // !defined(InterlockRule_H)
