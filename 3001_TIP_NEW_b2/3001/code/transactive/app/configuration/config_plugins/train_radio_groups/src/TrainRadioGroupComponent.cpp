/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/trainradiogroupcomponent.cpp $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */


#include "app/configuration/config_plugins/train_radio_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupComponent.h"
#include "app/configuration/config_plugins/train_radio_groups/src/resource.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDatabaseAccessor.h"
#include "app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupFrameworkView.h"

#include "bus/resources/resource.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    using TA_Base_Core::DebugUtil;

    
    const CString TrainRadioGroupComponent::DLL_NAME = "TA_TrainRadioGroupsConfigPlugin.dll";

    
    TrainRadioGroupComponent::TrainRadioGroupComponent( const std::string& componentName )
        : TA_Base_App::AbstractComponent( componentName, new TrainRadioGroupDatabaseAccessor() ),
          m_icon( NULL )
    {
        FUNCTION_ENTRY( "TrainRadioGroupComponent" );
        
        m_icon = LoadIcon( GetModuleHandle( DLL_NAME ), MAKEINTRESOURCE( IDI_CONFIG_SI_REGION_TALKGOUP ) );

        FUNCTION_EXIT;
    }


    TrainRadioGroupComponent::~TrainRadioGroupComponent()
    {
        FUNCTION_ENTRY( "~TrainRadioGroupComponent" );
        FUNCTION_EXIT;
    }

    unsigned long TrainRadioGroupComponent::getRightsActionForAddingThisComponent()
    {
        FUNCTION_ENTRY( "getRightsActionForAddingThisComponent" );
        FUNCTION_EXIT;
        return TA_Base_Bus::aca_CE_CONFIGURE_TRAIN_TALKGROUPS;
    }


    CRuntimeClass* TrainRadioGroupComponent::getComponentView()
    {
        FUNCTION_ENTRY( "getComponentView" );
        FUNCTION_EXIT;
        return RUNTIME_CLASS( TrainRadioGroupFrameworkView );
    }


    std::string TrainRadioGroupComponent::getDLLName() const
    {
        FUNCTION_ENTRY( "getDLLName" );
        FUNCTION_EXIT;
        return std::string( DLL_NAME );
    }


    HICON TrainRadioGroupComponent::getIcon()
    {
        FUNCTION_ENTRY( "getIcon" );
        FUNCTION_EXIT;
        return m_icon;
    }

}