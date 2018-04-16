#if !defined( TrainBorneManagerEntityData_H__INCLUDED_ )
#define TrainBorneManagerEntityData_H__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/CODE_TA_BASE_DDB/transactive/core/data_access_interface/entity_access/src/TrainBorneManagerEntityData.h $
  * @author zhang hongzhi 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/09/29 14:57:34 $
  * Last modified by: $Author: trainteam $
  * 
  * TrainBorneManagerEntityData provides access to TrainBorneManagerEntityData entity data 
  * and a specific framework for TrainBorneManagerEntityData entity parameters.
  */

#include <string>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class TrainBorneManagerEntityData : public EntityData
    {
    public:
        /**
          * TrainBorneManagerEntityData
          *
          * Constructs a new TrainBorneManagerEntityData object that contains the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        TrainBorneManagerEntityData();

        /**
          * TrainBorneManagerEntityData
          *
          * Constructs a new TrainBorneManagerEntityData object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this TrainBorneManagerEntityData in the database.
          */

        TrainBorneManagerEntityData( unsigned long key );

        /**
          * ~TrainBorneManagerEntityData
          *
          * Standard destructor.
          */

        virtual ~TrainBorneManagerEntityData() {};

        /**
          * getMaxScheduleMessage
          *
          * Gets the maximum number that TrainBorne Manager will queue message to train.  The number should be
          * limited by hardware, default is 6.
          *
          * @return     int
          *             Building poll interval, default is 500 milliseconds
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the AutoTrainBorneDirectory parameter cannot be converted to a string.
          */

        int getBulkingPollPeriod();
        

        ///////////////////////////////////////////////////////////////////////
        //
        // IEntityData Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getType
          *
          * Returns the type of this entity. This method must be callable when the object
          * is constructed using the default constructor. The entity type string should be
          * stored as a static const variable in the concrete class. The entity type should
          * not be loaded from the database (note that this method defines no exceptions).
          *
          * @return      std::string
          *              The entity type of this entity.
          */

	    virtual std::string getType();
	    static std::string getStaticType();

        /**
          * clone
          *
          * Returns a new instance of this entity. This method is used to create a concrete
          * entity based upon type in the EntityAccessFactory.
          *
          * @return     IEntityData*
          *             A new instance of this entity.  It is the caller's responsibility
          *             to clean up the object.
          *
          * @param      unsigned long key 
          *             The pkey of this entity in the database.
          */ 

        static IEntityData* clone( unsigned long key );
                
        /**
          * invalidate
          *
          * Marks the data contained by this entity as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate();

    protected:
         


    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;
       
        //
        // Disable copy constructor and assignment operator
        //

        TrainBorneManagerEntityData( const TrainBorneManagerEntityData& theTrainBorneManagerEntityData );
        TrainBorneManagerEntityData& operator=( const TrainBorneManagerEntityData& theTrainBorneManagerEntityData );

        /** 
          * validateData
          *
          * Reloads the data if invalidate() has been called.
          *
          * Preconditions:  0 != getHelper()
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if a parameter value cannot be converted to the required type.
          */

        void validateData();
   
        /** 
          * setParameter
          *
          * Sets the value for the named string parameter.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      const std::string& name
          *             The name of the parameter to set.
          * @param      const std::string& value
          *             The new parameter value.
          * @param      std::string& parameter
          *             The stored parameter to be set.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setParameter( const std::string& name, const std::string& value );

        //
        // The directories to which data is TrainBorned, including the full path. 
        //


    }; // TrainBorneManagerEntityData

    typedef boost::shared_ptr<TrainBorneManagerEntityData> TrainBorneManagerEntityDataPtr;
} // TA_Base_Core

#endif // !defined(TrainBorneManagerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
