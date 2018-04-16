/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an Ats Fixed Block Map's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_fixed_block_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapDetailView.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapDatabaseAccessor.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapListView.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/ats/src/AtsBranchTrackAccessFactory.h"
#include "core/data_access_interface/ecs/src/EcsZoneAccessFactory.h"
#include "core/data_access_interface/ats/src/IConfigAtsFbMapRecord.h"
#include "core/data_access_interface/ats/src/AtsFixedBlockMap.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/BVSStage.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    const int			AtsFixedBlockMapDetailView::ABSCISSA_FIELD_LENGTH	= 10;
	const std::string	AtsFixedBlockMapDetailView::ECS_ZONE_TYPE			= "EC_ZONES";			
	const std::string	AtsFixedBlockMapDetailView::CCTV_ZONE_TYPE			= "VI_ZONES";
	const std::string	AtsFixedBlockMapDetailView::RADIO_ZONE_TYPE			= "RC_ZONES";
    
    IMPLEMENT_DYNCREATE(AtsFixedBlockMapDetailView, CFormView)

    AtsFixedBlockMapDetailView::AtsFixedBlockMapDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AtsFixedBlockMapDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AtsFixedBlockMapDetailView)
	//}}AFX_DATA_INIT
    }


    AtsFixedBlockMapDetailView::~AtsFixedBlockMapDetailView()
    {
    }


    void AtsFixedBlockMapDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AtsFixedBlockMapDetailView)
	DDX_Control(pDX, IDC_EDIT_MIN_ABSCISSA, m_minAbscissaEdit);
	DDX_Control(pDX, IDC_COMBO_ZONE, m_zoneCombo);
	DDX_Control(pDX, IDC_COMBO_ZONETYPE, m_zoneTypeCombo);
	DDX_Control(pDX, IDC_EDIT_MAX_ABSCISSA, m_maxAbscissaEdit);
	DDX_Control(pDX, IDC_COMBO_BRANCH_ID, m_branchIdCombo);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	DDX_Control(pDX, IDC_EDIT_TRACK, m_trackEdit);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AtsFixedBlockMapDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AtsFixedBlockMapDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_CBN_KILLFOCUS(IDC_COMBO_BRANCH_ID, onKillfocusComboBranchId)
	ON_CBN_KILLFOCUS(IDC_COMBO_ZONE, OnKillfocusComboZone)
	ON_CBN_KILLFOCUS(IDC_COMBO_ZONETYPE, OnKillfocusComboZonetype)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_ABSCISSA, OnKillfocusEditMaxAbscissa)
	ON_EN_KILLFOCUS(IDC_EDIT_MIN_ABSCISSA, OnKillfocusEditMinAbscissa)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AtsFixedBlockMapDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
		m_zoneTypeCombo.AddString(ECS_ZONE_TYPE.c_str());
		m_zoneTypeCombo.AddString(CCTV_ZONE_TYPE.c_str());
		m_zoneTypeCombo.AddString(RADIO_ZONE_TYPE.c_str());

        m_minAbscissaEdit.SetLimitText(ABSCISSA_FIELD_LENGTH);
		m_maxAbscissaEdit.SetLimitText(ABSCISSA_FIELD_LENGTH);

        // Hide all the Ats Fixed Block Map details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AtsFixedBlockMapDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
		
		m_zoneTypeCombo.ShowWindow(isShown);
		m_zoneCombo.ShowWindow(isShown);
		m_trackEdit.ShowWindow(isShown);
		m_branchIdCombo.ShowWindow(isShown);
		m_minAbscissaEdit.ShowWindow(isShown);
		m_maxAbscissaEdit.ShowWindow(isShown);
        m_trackEdit.ShowWindow(isShown);
        m_type.ShowWindow(isShown);

		GetDlgItem(IDC_ZONETYPE_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_ZONE_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_TRACK_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_BRANCH_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_MIN_ABSCISSA_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_MAX_ABSCISSA_STATIC)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
		GetDlgItem(IDC_ZONETYPE_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_ZONE_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_TRACK_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_BRANCH_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_MIN_ABSCISSA_BITMAP)->ShowWindow(isShown);
		GetDlgItem(IDC_MAX_ABSCISSA_BITMAP)->ShowWindow(isShown);
    }


    void AtsFixedBlockMapDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AtsFixedBlockMapDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AtsFixedBlockMapListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AtsFixedBlockMapDetailView");
        }


        m_configItem = dynamic_cast<TA_IRS_Core::IConfigAtsFbMapRecord*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAtsFixedBlockMap* object

			AtsFixedBlockMapDatabaseAccessor::AtsFixedBlockMapRecord details =
				m_databaseAccessor->getItemDetails(m_configItem->getUniqueIdentifier());

			if( m_configItem->isNew() )
            {
                // This is a new Ats Fixed Block Map or at least a copy of an Ats Fixed Block Map so we indicate this in the type
                // field and make sure the modify and create dates are blank.

                m_type.SetWindowText(NEW_LABEL);
 
                // Disable the reset button as this is a new item so reset has no functionality
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

                // Enable the reset button for the user
                GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
            }

			// Zone Type
			m_zoneTypeCombo.SetCurSel(m_zoneTypeCombo.FindStringExact(-1, details.zoneType));

			// Zone ID
			// Populate the combo
			m_zoneCombo.ResetContent();

			std::string zoneType = details.zoneType;
			if (zoneType == ECS_ZONE_TYPE)
			{
				try
				{
					TA_IRS_Core::IECSZones ecsZones = TA_IRS_Core::ECSZoneAccessFactory::getInstance().getAllECSZones();
					
					for (TA_IRS_Core::IECSZonesIt it = ecsZones.begin();
						 it != ecsZones.end();
					     it++)
					{
						std::ostringstream zoneId;
						zoneId << (*it)->getECSLocation() << " - ";
						zoneId << (*it)->getTrack() << " (";
						zoneId << (*it)->getECSZoneId() << ")";
						CString zone = zoneId.str().c_str();
						m_zoneCombo.AddString(zone);
						delete (*it);
						(*it) = NULL;
					}
				}
				catch (...)
				{
				}
			}
			else if (zoneType == RADIO_ZONE_TYPE)
			{
				try
				{
					std::vector<TA_Base_Core::ILocation*> locations 
						= TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

					for (std::vector<TA_Base_Core::ILocation*>::iterator it = locations.begin();
						 it != locations.end();
					     it++)
					{
						std::ostringstream zoneId;
						zoneId << (*it)->getName().c_str() << " (";
						zoneId << (*it)->getKey() << ")"; 
						CString zone = zoneId.str().c_str();
						m_zoneCombo.AddString(zone);
						delete (*it);
						(*it) = NULL;
					}
				}
				catch (...)
				{
				}
			}
			else if (zoneType == CCTV_ZONE_TYPE)
			{
				try
				{
					TA_Base_Core::IEntityDataList bvsStages = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::BVSStage::getStaticType());
					
					for (TA_Base_Core::IEntityDataList::iterator it = bvsStages.begin();
						 it != bvsStages.end();
					     it++)
					{
						std::ostringstream zoneId;
						zoneId << (*it)->getName() << " (";
						zoneId << (*it)->getAddress() << ")";
						CString zone = zoneId.str().c_str();
						m_zoneCombo.AddString(zone);
						
						try
						{
							unsigned long bvsStage2 =  atoi((*it)->getAddress().c_str()) + 1;
							TA_Base_Core::IEntityDataList::iterator it2;
							for (it2 = bvsStages.begin(); it2 != bvsStages.end(); it2++)
							{
								if( atoi((*it2)->getAddress().c_str()) == bvsStage2 )
								{
									break;
								}
							}
							if(it2 != bvsStages.end())
							{
											
								std::ostringstream conflictZoneId;
								conflictZoneId << (*it)->getName() << "/" << (*it2)->getName();
								conflictZoneId << " Conflict (";
								conflictZoneId << (*it)->getAddress() << "." << (*it2)->getAddress() << ")";
								CString conflictZone = conflictZoneId.str().c_str();
								m_zoneCombo.AddString(conflictZone);

								std::ostringstream conflictZoneId2;
								conflictZoneId2 << (*it2)->getName() << "/" << (*it)->getName();
								conflictZoneId2 << " Conflict (";
								conflictZoneId2 << (*it2)->getAddress() << "." << (*it)->getAddress() << ")";
								conflictZone = conflictZoneId2.str().c_str();
								m_zoneCombo.AddString(conflictZone);

								std::ostringstream preConflictZoneId;
								preConflictZoneId << (*it)->getName() << "/" << (*it2)->getName();
								preConflictZoneId << " PreConflict (";
								preConflictZoneId << "-" << (*it)->getAddress() << "." << (*it2)->getAddress() << ")";
								conflictZone = preConflictZoneId.str().c_str();
								m_zoneCombo.AddString(conflictZone);
								
								std::ostringstream preConflictZoneId2;
								preConflictZoneId2 << (*it2)->getName() << "/" << (*it)->getName() ;
								preConflictZoneId2 << " PreConflict (";
								preConflictZoneId2 << "-" << (*it2)->getAddress() << "." << (*it)->getAddress() << ")";
								conflictZone = preConflictZoneId2.str().c_str();
								m_zoneCombo.AddString(conflictZone);
							}
						}
						catch (...)
						{
						}
					}
					for (TA_Base_Core::IEntityDataList::iterator it3 = bvsStages.begin(); it3 != bvsStages.end(); it3++)
					{
						delete *it3;
					}

				}
				catch (...)
				{
				}
			}

			if (details.zoneId == "")
            {
                m_zoneCombo.SetCurSel(-1);
            }
            else
            {
				m_zoneCombo.SetCurSel(m_zoneCombo.FindStringExact(-1, details.zoneId));
            }

			// Track
            m_trackEdit.SetWindowText( details.track );
            m_trackEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            m_trackEdit.EnableWindow(false);

			// Branch ID
			// Get all the items for the combo
			try
			{
				TA_IRS_Core::IAtsBranchTracks branchTracks = 
					TA_IRS_Core::AtsBranchTrackAccessFactory::getInstance().getAllAtsBranchTracks();

				m_branchIdCombo.ResetContent();

				for (TA_IRS_Core::IAtsBranchTracksIt it = branchTracks.begin();
					 it != branchTracks.end();
					 it++)
				{
					m_branchIdCombo.AddString((*it)->getAtsBranchId().c_str());
					delete (*it);
					(*it) = NULL;
				}
			}
			catch (...)
			{
			}
				
            if (details.branchId == "")
            {
                m_branchIdCombo.SetCurSel(-1);
            }
            else
            {
				m_branchIdCombo.SetCurSel(m_branchIdCombo.FindStringExact(-1, details.branchId));
            }

			// Min Abscissa
			m_minAbscissaEdit.SetWindowText( details.minAbscissa );
            m_minAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            m_minAbscissaEdit.EnableWindow(true);

			// Min Abscissa
			m_maxAbscissaEdit.SetWindowText( details.maxAbscissa );
            m_maxAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            m_maxAbscissaEdit.EnableWindow(true);
     
            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Ats Fixed Block Map details to display");
            
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// ++ TD14164
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Ats Fixed Block Map details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Ats Fixed Block Map";
			userMsg << fieldName;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030067);
			// ++ TD14164
        }

        FUNCTION_EXIT;
    }

    void AtsFixedBlockMapDetailView::refreshButtonAvailability()
    {
        if (m_configItem == NULL)
        {
            return;
        }

        if (m_configItem->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_configItem->hasChanged())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    }

	void AtsFixedBlockMapDetailView::OnKillfocusComboZonetype() 
	{
        FUNCTION_ENTRY("OnKillfocusComboZonetype");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString zoneTypeCString;
		
		if (m_zoneTypeCombo.GetCurSel() >= 0)
		{
			m_zoneTypeCombo.GetLBText(m_zoneTypeCombo.GetCurSel(), zoneTypeCString);
		}
		else
		{
			zoneTypeCString = "";
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (zoneTypeCString == m_configItem->getZoneTypeName().c_str())
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setZoneTypeName( (LPCTSTR) zoneTypeCString );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Fixed Block Map could not be found in the database");

			m_zoneTypeCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

		populateItemDetails(m_configItem);

        FUNCTION_EXIT;
    }

	void AtsFixedBlockMapDetailView::OnKillfocusComboZone() 
	{
        FUNCTION_ENTRY("OnKillfocusComboZone");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString zoneCString;
		
		if (m_zoneCombo.GetCurSel() >= 0)
		{
			m_zoneCombo.GetLBText(m_zoneCombo.GetCurSel(), zoneCString);
		}
		else
		{
			zoneCString = "";
		}

		std::string zoneString = zoneCString;
		std::string::size_type pos = zoneString.find_last_of("(");
		double zoneId = -1;

		if (pos != std::string::npos)
		{
			std::istringstream zoneStream;
			zoneStream.str(zoneString.substr(pos+1)); // Skip the bracket;
			zoneStream >> zoneId;
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (zoneId == -1 || zoneId == m_configItem->getZoneId())
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setZoneId( zoneId );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Fixed Block Map could not be found in the database");

			m_zoneCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void AtsFixedBlockMapDetailView::onKillfocusComboBranchId()
	{
        FUNCTION_ENTRY("onKillfocusComboBranchId");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString branchIdString;
		
		if (m_branchIdCombo.GetCurSel() >= 0)
		{
			m_branchIdCombo.GetLBText(m_branchIdCombo.GetCurSel(), branchIdString);
		}
		else
		{
			branchIdString = "";
		}

        try
        {
            // If the track string has not changed then there is no point doing an update
            if (branchIdString == m_configItem->getBranchIdAsString().c_str())
            {
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setBranchIdAsString( (LPCTSTR) branchIdString );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Fixed Block Map could not be found in the database");

			m_branchIdCombo.SetCurSel(-1);

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

		populateItemDetails(m_configItem);

        FUNCTION_EXIT;
    }

	void AtsFixedBlockMapDetailView::OnKillfocusEditMaxAbscissa() 
	{
		FUNCTION_ENTRY("OnKillfocusEditMaxAbscissa");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString maxAbscissaCString;
        m_maxAbscissaEdit.GetWindowText(maxAbscissaCString);
		std::string maxAbscissaString = maxAbscissaCString;
		std::istringstream maxAbscissaStream; 
		maxAbscissaStream.str(maxAbscissaString);
		unsigned long maxAbscissa;
		if (!(maxAbscissaStream >> maxAbscissa))
		{
			// Failed - clear out
			m_maxAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

			FUNCTION_EXIT;
            return;
		}

        try
        {
            // If the branchId has not changed then there is no point doing an update
            if (m_configItem->getMaxAbscissa() == maxAbscissa)
            {
                m_maxAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setMaxAbscissa( maxAbscissa );
            m_maxAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Fixed Block Map could not be found in the database");

            m_maxAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;	
	}

	void AtsFixedBlockMapDetailView::OnKillfocusEditMinAbscissa() 
	{
		FUNCTION_ENTRY("OnKillfocusEditMinAbscissa");
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString minAbscissaCString;
        m_minAbscissaEdit.GetWindowText(minAbscissaCString);
		std::string minAbscissaString = minAbscissaCString;
		std::istringstream minAbscissaStream; 
		minAbscissaStream.str(minAbscissaString);
		unsigned long minAbscissa;
		if (!(minAbscissaStream >> minAbscissa))
		{
			// Failed - clear out
			m_minAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

			FUNCTION_EXIT;
            return;
		}

        try
        {
            // If the branchId has not changed then there is no point doing an update
            if (m_configItem->getMinAbscissa() == minAbscissa)
            {
                m_minAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                FUNCTION_EXIT;
                return;
            }

            m_configItem->setMinAbscissa( minAbscissa );
            m_minAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The Ats Fixed Block Map could not be found in the database");

            m_minAbscissaEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }

	    m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(),"");
        refreshButtonAvailability();

        FUNCTION_EXIT;	
	}


    void AtsFixedBlockMapDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_configItem);

        FUNCTION_EXIT;
    }


    void AtsFixedBlockMapDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

		if ( !m_minAbscissaEdit.isTextValid() ||
			 !m_maxAbscissaEdit.isTextValid() ||
			 m_zoneCombo.GetCurSel() == -1 ||
			 m_zoneTypeCombo.GetCurSel() == -1 ||
			 m_branchIdCombo.GetCurSel() == -1 )
        {
            //UE-030060
			// TD14164 ++
            /*MessageBox("The item could not be applied because invalid names still exist in the fields.",
                       RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "invalid names still exist in the fields";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030068);
			// ++ TD14164
            return;
        }

        applyButtonPressed(m_configItem);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_configItem->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AtsFixedBlockMapDetailView diagnostics

    #ifdef _DEBUG
    void AtsFixedBlockMapDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AtsFixedBlockMapDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
