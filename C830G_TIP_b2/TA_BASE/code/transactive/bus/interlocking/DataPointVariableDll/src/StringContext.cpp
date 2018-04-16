/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/DataPointVariableDll/src/StringContext.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include "stdafx.h"
#include "StringContext.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"

namespace TA_Base_Bus
{
    /**
    * Constructor
    * 
    */
    StringContext::StringContext()
    {
    }

    /**
    * Destructor
    * 
    */
    StringContext::~StringContext()
    {
    }

    /**
     * getValue
     *
     * Gets the value of a variable.
     *
     * @exception MathematicalEvaluationException - if variable cannot be found
     */
    std::string StringContext::getValue(const std::string& variableName) const
    {
        TA_Base_Bus::DataPointCorbaDef::ClientDataPointState_var dataPointState;

        try
        {
            m_dataPoint.setEntityName( variableName );

            CORBA_CALL_RETURN( dataPointState, m_dataPoint, GetDataPointState, () );

            if( dataPointState->value._d() == TA_Base_Bus::DataPointCorbaDef::dpvText )
            {
                TA_Base_Bus::DataPointCorbaDef::ByteSequence readSeq = dataPointState->value.stringValue();
			    std::string dataPointValue = "";
                int length = readSeq.length();

			    for ( int i = 0; i < length; i++ )
			    {
				    dataPointValue += (char) readSeq[i];
			    }
                return dataPointValue;
            }
            else
            {
                // the data type of the data point is not textual
                std::stringstream errorMsg;
                errorMsg << "Attempting to use a string context for the non-textual data point ";
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
            errorMsg << "An error occured while attempting to evaluate the textual value of the data point ";
            errorMsg << variableName;
            TA_THROW( TA_Base_Core::MathematicalEvaluationException( errorMsg.str().c_str() ) );
        }
    }

}
