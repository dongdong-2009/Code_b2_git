/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_entity/src/InterlockVariable.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */


#if !defined(InterlockVariable_H)
#define InterlockVariable_H

#include <string>

#include "core/data_access_interface/interlocking_variable/src/InterlockVariableHelper.h"
#include "core/data_access_interface/interlocking_variable/src/IInterlockVariable.h"

namespace TA_Base_Core
{
    class InterlockVariable : public IInterlockVariable
    {

    public:

        /**
          * Constructor
          */
        InterlockVariable(const std::string entityType);


        virtual ~InterlockVariable();


    public:
        /**
          * getName
          *
          * Returns the name of the DLL that contains the interlocking support code for this entity type.
          *
          * @return The name of a dll
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getDllName();

        /**
          * getName
          *
          * Returns the name of the DLL that contains the interlocking support code for this entity type.
          *
          * @return The name of a dll
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getEntityType();
 
        /**
          * getName
          *
          * Returns the name of the DLL that contains the interlocking support code for this entity type.
          *
          * @return The name of a dll
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, or if the wrong ammount of data is retrieved.
          */
        virtual std::string getButtonText();

    private:

        // Assignment operator not used so it is made private
		InterlockVariable& operator=(const InterlockVariable &);
		
        // Copy constructor should not be used and has therefore been made private. 
        InterlockVariable( const InterlockVariable& theVariable);  

        TA_Base_Core::InterlockVariableHelper * m_InterlockVariableHelper;
    };
} // closes TA_Base_Core

#endif // !defined(InterlockVariable_H)
