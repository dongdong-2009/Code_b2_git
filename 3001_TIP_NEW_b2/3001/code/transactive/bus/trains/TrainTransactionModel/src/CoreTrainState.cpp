/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "bus/trains/TrainTransactionModel/src/CoreTrainState.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    const CommonTypes::ETrainResource CoreTrainState::RESOURCE_CATEGORY = CommonTypes::CoreResource;


    CoreTrainState::CoreTrainState( CommonTypes::TrainIdType trainId,
                                    const std::string& radioId,
                                    unsigned long location )
       : TrainState( trainId ),
         m_radioId( radioId ),
         m_currentLocation( location )
    {
        FUNCTION_ENTRY( "CoreTrainState" );
        FUNCTION_EXIT;
    }


    CoreTrainState::~CoreTrainState()
    {
        FUNCTION_ENTRY( "~CoreTrainState" );
        FUNCTION_EXIT;
    }


    CommonTypes::ETrainResource CoreTrainState::getResourceCategory() const
    {
        FUNCTION_ENTRY( "CoreTrainState" );
        FUNCTION_EXIT;
        return RESOURCE_CATEGORY;
    }


    bool CoreTrainState::isValid() const
    {
        FUNCTION_ENTRY( "CoreTrainState" );
        FUNCTION_EXIT;
        return m_isValid;
    }


    std::string CoreTrainState::getRadioId() const
    {
        FUNCTION_ENTRY( "CoreTrainState" );
        FUNCTION_EXIT;
        return m_radioId;
    }


    unsigned long CoreTrainState::getLocation() const
    {
        FUNCTION_ENTRY( "CoreTrainState" );
        FUNCTION_EXIT;
        return m_currentLocation;
    }


    void CoreTrainState::setValidity( bool isValid )
    {
        FUNCTION_ENTRY( "CoreTrainState" );

        // sets the validity compares the old value, if different setDataChanged()

        if ( isValid != m_isValid )
        {
            m_isValid = isValid;
            setDataChanged();
        }

        FUNCTION_EXIT;
    }


    void CoreTrainState::setRadioId( const std::string& radioId )
    {
        FUNCTION_ENTRY( "CoreTrainState" );

        // sets the radio ID compares the old value, if different setDataChanged()

        if ( radioId != m_radioId )
        {
            m_radioId = radioId;
            setDataChanged();
        }

        FUNCTION_EXIT;
    }


    void CoreTrainState::setLocation( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "CoreTrainState" );

        //sets the location compares the old value, if differentsetDataChanged()

        if ( locationKey != m_currentLocation )
        {
            m_currentLocation = locationKey;
            setDataChanged();
        }

        FUNCTION_EXIT;
    }
}
