/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_entity/src/InterlockEntity.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */


#if !defined(InterlockEntity_H)
#define InterlockEntity_H

#include <string>

#include "core/data_access_interface/interlocking_entity/src/IInterlockEntity.h"

namespace TA_Base_Core
{
    class InterlockEntityHelper;

    /**
     * The concreate implementation of the IInterlockEntity class
     * Used to retrieve entity information from the db
     */
    class InterlockEntity : public IInterlockEntity
    {

    public:

        /**
          * Constructor
          */
        InterlockEntity(const std::string& entityType);


        virtual ~InterlockEntity();


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

    private:

        // Assignment operator not used so it is made private
		InterlockEntity& operator=(const InterlockEntity &);
		
        // Copy constructor should not be used and has therefore been made private. 
        InterlockEntity( const InterlockEntity& theEntity);  

        TA_Base_Core::InterlockEntityHelper * m_interlockEntityHelper;
    };
} // closes TA_Base_Core

#endif // !defined(InterlockEntity_H)
