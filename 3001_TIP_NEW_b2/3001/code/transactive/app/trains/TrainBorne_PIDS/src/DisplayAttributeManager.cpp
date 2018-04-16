/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/DisplayAttributeManager.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/16 18:12:41 $
  * Last modified by:  $Author: bjteamleads $
  *
  * This handles the time and priority controls
  * on the display page.
  *
  */


#include "StdAfx.h"
#include "DisplayAttributeManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <sstream>


namespace TA_IRS_App
{
	//perRun must be in 1 to 255, 0 mean continuous 
	const unsigned char DisplayAttributeManager::MAX_PER_RUN = 255;
	const unsigned char DisplayAttributeManager::MIN_PER_RUN = 1;


    using TA_Base_Core::DebugUtil;
	DisplayAttributeManager::DisplayAttributeManager( CButton& timeTypeContinuous,
							 CButton& timeTypePerrun,
                             ValidatingEdit& perRun,
							 CStatic& timeTypeContinuousLabel,
							 CStatic& timeTypePerrunLabel ):
          m_timeTypeContinuous( &timeTypeContinuous ),
          m_timeTypePerrun( &timeTypePerrun),
          m_perRun( &perRun),
          m_timeTypeContinuousLabel( &timeTypeContinuousLabel ),
          m_timeTypePerrunLabel( &timeTypePerrunLabel )
    {
        FUNCTION_ENTRY( "DisplayAttributeManager" );

        // do the initial GUI setup

        // set the repeat interval
        m_perRun->setAllowedCharacters( "0123456789" );

        //setPerrun(0);

        // blank everything out to start

        FUNCTION_EXIT;
    }


    DisplayAttributeManager::~DisplayAttributeManager()
    {
        FUNCTION_ENTRY( "~DisplayAttributeManager" );


        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::onTimeTypeContinuous()
    {
        FUNCTION_ENTRY( "onTimeTypeContinuous" );

        m_timeType = MESSAGE_CONTINUOUS;
        m_timeTypePerrun->SetCheck( 0 );
        m_timeTypeContinuous->SetCheck( 1 );
        timeTypeChanged();

        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::onTimeTypePerrun()
    {
        FUNCTION_ENTRY( "onTimeTypePerrun" );

        m_timeType = MESSAGE_PERRUN;
        m_timeTypeContinuous->SetCheck( 0 );
        m_timeTypePerrun->SetCheck( 1 );
		// once perrun select, the value should be 1
		setPerrun(1);
        timeTypeChanged();

        FUNCTION_EXIT;
    }

	void DisplayAttributeManager::setContinuousBtnCheck()
	{
        onTimeTypeContinuous();
	}

	void DisplayAttributeManager::setPerrunBtnCheck()
	{
        onTimeTypePerrun();
	}

	void DisplayAttributeManager::setPerrun( unsigned short perRun,
            bool canChange /* = true */ )
    {
        FUNCTION_ENTRY( "setRepeatInterval" );
        // set the repeat interval
        std::stringstream ss;
        ss << perRun;
        std::string sperRun;
        ss >> sperRun;
        // if perRun == 0, blank the edit ctrl
		if(perRun == 0)
		{
			sperRun = "";
		}

        m_perRun->SetWindowText( sperRun.c_str() );

        // the repeat interval is only enabled if this is true, and the current
        // time type is cyclic
		/*
        m_perRunEnabled = canChange;
        m_perRun->EnableWindow( m_perRunEnabled&&
                                        ( m_timeType == MESSAGE_PERRUN) );
        m_timeTypePerrunLabel->EnableWindow( m_perRunEnabled &&
                                             ( m_timeType == MESSAGE_PERRUN) );
	    */

        FUNCTION_EXIT;
    }


    DisplayAttributeManager::TimeType DisplayAttributeManager::getMessageType() const
    {
        FUNCTION_ENTRY( "getMessageType" );
        FUNCTION_EXIT;
        return m_timeType;
    }


    unsigned short DisplayAttributeManager::getPerrun() const
    {
        FUNCTION_ENTRY( "getRepeatInterval" );

		CString perrun;
        m_perRun->GetWindowText( perrun );

        std::stringstream ss;
        ss.str( LPCTSTR( perrun ) );
        unsigned short value = 0;
        ss >> value;
        FUNCTION_EXIT;

        return value;
    }

	bool DisplayAttributeManager::isPerrunInRange()
	{
		unsigned short value = getPerrun();
		return (value >= MIN_PER_RUN) && (value <= MAX_PER_RUN);
	}

    void DisplayAttributeManager::timeTypeChanged()
    {
        FUNCTION_ENTRY( "timeTypeChanged" );
		/*
        switch ( m_timeType )
        {
            case MESSAGE_PERRUN:
                m_timeTypePerrunLabel->EnableWindow( m_perRunEnabled );
                m_perRun->EnableWindow( m_perRunEnabled );
                break;

            case MESSAGE_CONTINUOUS:
                // continuous radio button selected
				m_timeTypePerrunLabel->EnableWindow( FALSE );
                m_perRun->EnableWindow( FALSE );
                break;
        }
		*/

        FUNCTION_EXIT;
    }


} // TA_IRS_App

