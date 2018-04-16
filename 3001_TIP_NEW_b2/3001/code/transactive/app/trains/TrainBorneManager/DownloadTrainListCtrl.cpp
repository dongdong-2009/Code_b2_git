/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/DownloadTrainListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2011/01/04 18:37:39 $
  * Last modified by: $Author: huang.wenbo $
  * 
  * DownloadTrainListCtrl defines a list control that displays trains
  * and the message library/time schedule version.
  */


#include "stdafx.h"

#include "resource.h"

#include "DownloadTrainListCtrl.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;

namespace TA_IRS_App
{

    // the names of the custom columns
    const std::string DownloadTrainListCtrl::VERSION_COLUMN = "Current Version";
    //hongran++ TD17552
    const std::string DownloadTrainListCtrl::UPGRADED_COLUMN = "Next Version";
    //++hongran TD17552

    //TD17545
    const std::string DownloadTrainListCtrl::TRAIN_SETTINGS_ENTITY_TYPE = "TrainSettings";
    const std::string DownloadTrainListCtrl::TRAIN_SETTINGS_ENTITY_NAME = "TrainAgentSettings";
    //TD17545

	DownloadTrainListCtrl::DownloadTrainListCtrl() 
		:TrainListCtrl("DownloadTrainListCtrl"),
        m_versionColumnIndex(-1),
       m_upgradedColumnIndex(-1),
       m_totalTrainNumber(40) //TD17545
    {
        FUNCTION_ENTRY( "DownloadTrainListCtrl" );

        //TD17545
        try
        {
            std::vector<TA_Base_Core::IEntityData*> trainSettingEntityData =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(TRAIN_SETTINGS_ENTITY_TYPE, TRAIN_SETTINGS_ENTITY_NAME);

            int size = trainSettingEntityData.size();

            if (size <= 0)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"No data get for TrainSettingEntityData. The total train number will be default 40");
            }
            else
            {
                m_totalTrainNumber = dynamic_cast<TA_Base_Core::TrainSettingsEntityData*>(trainSettingEntityData[0])->getTotalTrainNumber();
            }
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,"Error when try to access TrainSettingEntityData. The total train number will be default 40");
        }

        //TD17545

        FUNCTION_EXIT;
    }


    DownloadTrainListCtrl::~DownloadTrainListCtrl()
    {
        FUNCTION_ENTRY( "~DownloadTrainListCtrl" );
        TA_IRS_Bus::LocationCache::getInstance().deregisterObserver(this);
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(DownloadTrainListCtrl, TA_IRS_Bus::TrainListCtrl)
        //{{AFX_MSG_MAP(DownloadTrainListCtrl)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void DownloadTrainListCtrl::setupList(TA_IRS_Bus::ICustomTrainDataProvider* versionProvider)
    {
        FUNCTION_ENTRY( "setupList" );

        // set the data provider (for version info)
        setCustomDataProvider(versionProvider);

        // set the standard column sizes
		// if set column size to 0, the column will not show
		// for 955 the LOCATION column and TETRA_ID column should not show
        setStandardColumnSizes(0, 120, 0);

		// change column name from "PV/Service" to "TrainID/Service"
        LVCOLUMN column;
		GetColumn(1, &column);
		column.pszText = "TrainID/Service";
		SetColumn(1, &column);

        // set up custom columns, current version & next version
        m_versionColumnIndex = addCustomColumn(VERSION_COLUMN, 120);
        m_upgradedColumnIndex = addCustomColumn(UPGRADED_COLUMN, LVSCW_AUTOSIZE_USEHEADER);
        TA_IRS_Bus::LocationCache::getInstance().registerObserver(this); //for: isSelectable --> LocationCache

        FUNCTION_EXIT;
    }


    void DownloadTrainListCtrl::updateTrainVersion( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "updateTrainVersion" );

        // if the columns have been added
        if ( (m_versionColumnIndex > -1) && (m_upgradedColumnIndex > -1) )
        {
            // just force a refresh
            refreshCustomColumn(trainId, VERSION_COLUMN);
            refreshCustomColumn(trainId, UPGRADED_COLUMN);
        }

        FUNCTION_EXIT;
    }
   

    std::string DownloadTrainListCtrl::getServiceNumberForTrain(TA_IRS_Bus::CommonTypes::TrainIdType trainId)
    {
        FUNCTION_ENTRY( "getServiceNumberForTrain" );

        // find the train
        int index = findTrainIndex(trainId);

        // get the structure, and return the service string
        if (index > -1)
        {
            DWORD itemData = GetItemData(index);

            FUNCTION_EXIT;
            return reinterpret_cast<TA_IRS_Bus::TrainListCtrl::TrainDetailsItemData*>(itemData)->serviceNumber;
        }

        // not found - dont have information for this train
        FUNCTION_EXIT;
        return "_";
    }


    bool DownloadTrainListCtrl::isSelectable(int rowNumber)
    {
        FUNCTION_ENTRY( "isSelectable" );

        DWORD itemData = GetItemData(rowNumber);

        FUNCTION_EXIT;
        return isSelectable(*reinterpret_cast<TA_IRS_Bus::TrainListCtrl::TrainDetailsItemData*>(itemData));
    }


    bool DownloadTrainListCtrl::isSelectable(const TA_IRS_Bus::TrainListCtrl::TrainDetailsItemData& itemData) const
    {
        FUNCTION_ENTRY( "isSelectable" );

        TA_IRS_Bus::LocationCache& locationCache = TA_IRS_Bus::LocationCache::getInstance();

        // get the GUI location and location type
        unsigned int currentLocation = locationCache.getLocationKey();
        TA_Base_Core::ILocation::ELocationType currentLocationType = locationCache.getLocationTypeByKey( currentLocation );

        // if this is a station, nothing is selectable
        // DO NOT CHANGE THIS, if this is selectable it will allow the operator to attempt to download/upgrade which will fail
        // See the TIS Gui Design Note, and the comments indicating this
        // The TIS and train agents are designed not to allow this function at a station ESPECIALLY IN DEGRADED MODE
        // If this is done in degraded mode then no other station will be aware of the train's new version
        if ( TA_Base_Core::ILocation::STATION == currentLocationType )
        {
            FUNCTION_EXIT;
            return false;
        }

        //limin++ CL-17925
        if ( false == TA_IRS_Bus::LocationCache::getInstance().isInControlOfLocation( itemData.currentLocation ) )
        {
            FUNCTION_EXIT;
            return false;
        }
        //++limin CL-17925

        // this must be the OCC or the depot location

        // the train is selectable here if
        // 1. it is in the local ATS train table, or
        // 2. The train is communicating with the local agent

        // for train location we only need to check ATS validity, if it is true, then it means it is in the local ATS train table (may not be localised),
        // it may not be localised, it may be in the workshop etc
        if ( ( true == itemData.atsValid ) || // in local ATS train table?
            ( ( true == itemData.communicationValid ) && // communicating with an agent?
              ( currentLocation == itemData.communicatingLocation ) ) ) // is it this agent?
        {
            FUNCTION_EXIT;
            return true;
        }


        FUNCTION_EXIT;
        return false;
    }


} // TA_IRS_App

