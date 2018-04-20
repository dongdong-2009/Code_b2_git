/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_variable/src/InterlockVariableHelper.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(InterlockVariableHelper_H)
#define InterlockVariableHelper_H


namespace TA_Base_Core
{
    /**
     * Helper class does all the nitty gritty stuff
     */
    class InterlockVariableHelper
    {

    public:
        /**
         * InterlockVariableHelper
         * 
         * This constructor creates a new InterlockVariableHelper for the specified key.
         *
         * @param entityType the type of entity to search for
         */
        InterlockVariableHelper(const std::string& entityType);


        /**
         * InterlockVariableHelper
         * 
         * Copy Constructor
         */
        InterlockVariableHelper( const InterlockVariableHelper& theInterlockVariableHelper);


        virtual ~InterlockVariableHelper();

    public:

        /**
         * getDllName
         *
         * Returns the name of the dll that contains the support code for this interlocking entity type.
         * If this is the first time data has been requested, the data is loaded from the database.
         *
         * @return The name for the dll as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount of data is retrieved.
         *
         */
        std::string getDllName();

        /**
         * getDllName
         *
         * Returns the name of the dll that contains the support code for this interlocking entity type.
         * If this is the first time data has been requested, the data is loaded from the database.
         *
         * @return The name for the dll as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount of data is retrieved.
         *
         */
        std::string getEntityType();

        /**
         * getDllName
         *
         * Returns the name of the dll that contains the support code for this interlocking entity type.
         * If this is the first time data has been requested, the data is loaded from the database.
         *
         * @return The name for the dll as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, or if the wrong ammount of data is retrieved.
         *
         */
        std::string getButtonText();


    private:
        // Made private as it is not used
		InterlockVariableHelper& operator=(const InterlockVariableHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockVariableData() has already been called
         */
        void reload();


        std::string m_dllName;
        std::string m_entityType;
        std::string m_buttonText;

        bool m_isValidData;

    };
} // closes TA_Base_Core

#endif // !defined(InterlockVariableHelper_H)
