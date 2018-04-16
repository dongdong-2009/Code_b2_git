/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/derived_datapoints/src/DerivedDataPointEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  *
  * DerivedDataPointEntityData provides read-only parameters that are specific
  * to a Derived DataPoint entity in database
  */


#ifndef DERIVED_DATAPOINT_ENTITY_DATA_H
#define DERIVED_DATAPOINT_ENTITY_DATA_H

#include <string>
#include <vector>
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointHelper.h"

namespace TA_Base_Core
{
    class ConfiguredInputAssociation;
    class DerivedOutputAssociation;
	class ConfiguredDerivedState;
	class DerivedDataPointHelper;


    class DerivedDataPointEntityData
    {
	public:

		DerivedDataPointEntityData ( unsigned long derivedDataPoint_pkey,
									 const std::string & derivedDataPoint_name);

        virtual ~DerivedDataPointEntityData();

		//
		// operations specific to DerivedDataPointEntityData
		//

        /**
         * getConfiguredInputAssociations
         *
         * Get the list of input entities associated with the DerivedDataPoint.  Caller of
         * this operation is responsible for the lifecycle of the returned objects.  Empty
         * list is returned if there is no input entities associated with the DerivedDataPoint
         *
         * @param listOfInputAssociations  The list of ConfiguredInputAssociation objects
         *
         */
		void getInputAssociations ( std::vector < ConfiguredInputAssociation * > & listOfInputAssociations );

        /**
         * getConfiguredDerivedStates
         *
         * Get the list of pre-defined states associated with the DerivedDataPoint.  Caller of
         * this operation is responsible for the lifecycle of the returned objects.  Empty
         * list is returned if there is no derived states defined for the DerivedDataPoint.
         *
         * @param listOfDerivedStates  The list of ConfiguredDerivedState objects
         *
         */
		void getConfiguredDerivedStates ( std::vector < ConfiguredDerivedState * > & listOfDerivedStates );

    private:

        /**
         * getDerivedOutputAssociation
         *
         * Get the output entity associated with the specified derived stated.  Caller of
         * this operation is responsible for the lifecycle of the returned objects.  Null
         * point is returned if there is no output entity configured for the derived state.
         *
         * @param derivedState_pkey		The primary key of a derived state
		 * @param listOfOutputEntities	The list of output objects associated with
		 *								a DerivedState
         *
         */
		void getDerivedOutputAssociation ( unsigned long derivedState_pkey, std::vector < DerivedOutputAssociation * > & listOfOutputEntities );

        DerivedDataPointEntityData() {};
        DerivedDataPointEntityData ( const DerivedDataPointEntityData & obj ){};
        DerivedDataPointEntityData & operator= ( const DerivedDataPointEntityData &);

		unsigned long m_pkey;
		std::string m_name;
		DerivedDataPointHelper m_derivedDataPointHelper;
    };
} //close namespace TA_Base_Core

#endif

