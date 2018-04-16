/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/DataPointVariableDll/src/BooleanContext.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * BooleanContext is a class that provides resolution of boolean datapoint variables.
  */

#include "stdafx.h"
#include "BooleanContext.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"

namespace TA_Base_Bus
{
   /**
	* Constructor
	* 
	*/
    BooleanContext::BooleanContext()
    {
    }

   /**
	* Destructor
	* 
	*/
    BooleanContext::~BooleanContext()
    {
    }

    /**
     * getValue
     *
     * Gets the value of a variable. 
     *
     * @exception MathematicalEvaluationException - if variable cannot be found
     */
    bool BooleanContext::getValue(const std::string& variableName) const
    {
        TA_Base_Bus::DataPointCorbaDef::ClientDataPointState_var dataPointState;

        try
        {

            m_dataPoint.setEntityName( variableName );

			CORBA_CALL_RETURN( dataPointState,
			                   m_dataPoint,
			                   GetDataPointState,
			                   () );

        

            if( dataPointState->value._d() == TA_Base_Bus::DataPointCorbaDef::dpvBoolean )
            {
                return dataPointState->value.booleanValue();
            }
            else
            {
                // the data type of the data point is not boolean
                std::stringstream errorMsg;
                errorMsg << "Attempting to use a boolean context for the non-boolean data point ";
                errorMsg << variableName;
                TA_THROW( TA_Base_Core::MathematicalEvaluationException( errorMsg.str().c_str() ) );
            }
        }
        catch( TA_Base_Core::ObjectResolutionException )
        {
            throw;
        }
        catch( ... )
        {
            std::stringstream errorMsg;
            errorMsg << "An error occured while attempting to evaluate the boolean value of the data point ";
            errorMsg << variableName;
            TA_THROW( TA_Base_Core::MathematicalEvaluationException( errorMsg.str().c_str() ) );
        }
    }

}
