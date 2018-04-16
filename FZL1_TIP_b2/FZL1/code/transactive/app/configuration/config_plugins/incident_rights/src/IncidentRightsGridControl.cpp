/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/incident_rights/src/IncidentRightsGridControl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// IncidentRightsGridControl.cpp: implementation of the IncidentRightsGridControl class.
//
//////////////////////////////////////////////////////////////////////

 
#include "app/configuration/config_plugins/incident_rights/src/stdafx.h"
#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsGridControl.h"
#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsComponent.h"
#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsView.h"
#include "app/configuration/config_plugins/incident_rights/src/resource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/data_access_interface/src/ProfileGroupAccessFactory.h"
#include "core/data_access_interface/src/ProfileGroup.h"
 
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App::TA_Configuration;
//using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace std;

namespace TA_IRS_App
{
 
	IncidentRightsGridControl::IncidentRightsGridControl()
		: m_bDataChanged(false), m_bIsReset(false)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}
	
	IncidentRightsGridControl::~IncidentRightsGridControl()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
		
	}
	
BEGIN_MESSAGE_MAP(IncidentRightsGridControl, CGridCtrl)
	//{{AFX_MSG_MAP(RightsGridControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	void IncidentRightsGridControl::PreSubclassWindow() 
    {
	    SetEditable(TRUE);
        SetDoubleBuffering(TRUE);
	    EnableDragAndDrop(FALSE);

        SetFixedRowCount(1);    // This is the row for subsystems
		SetFixedColumnCount(1); // This is the column of profiles

        // Set the background colour of the control so we cannot see where the background if the columns and rows are
        // smaller than the control
        SetBkColor( GetSysColor(COLOR_3DFACE) );
 
        CGridCtrl::PreSubclassWindow();
    }

	void IncidentRightsGridControl::setUpHeaders()
    {
        FUNCTION_ENTRY("setUpHeaders");
 
        // Set up the standard item fields so we don't have to do it each time we insert an item
		GV_ITEM item;
        item.mask = GVIF_TEXT|GVIF_FORMAT;
		item.col = 0;
		item.nFormat = DT_LEFT|DT_WORDBREAK;

        //
        // Set up the accessor profile headings
        //
		std::map<unsigned long,std::string> accessProfilesGroups = getAllProfileGroups();
		std::map<unsigned long,std::string>::iterator  profileIter;
        
		SetRowCount(accessProfilesGroups.size()+1);
        // We ignore the first row (0) because it is the corner cell
        int row = 1;

        for (profileIter = accessProfilesGroups.begin(); profileIter != accessProfilesGroups.end(); 
				++profileIter)
        {
			item.row = row;
            item.szText = profileIter->second.c_str();

			SetItem(&item);
            SetItemData(item.row,item.col,profileIter->first);            
            ++row;
        }

        // Sort the first column so that all the profiles are in alphabetical order.
        SortTextItems(0,TRUE);

        //
        // Set up the creator profile group headings
		// we still make use of the same access profile groups
        //        
        SetColumnCount(accessProfilesGroups.size()+1);

        // We ignore the first column (0) because it is the corner cell
        int column = 1;
        item.row = 0;

       for (profileIter = accessProfilesGroups.begin(); profileIter != accessProfilesGroups.end(); 
				++profileIter)
        {
			item.col = column;
            item.szText = profileIter->second.c_str();

			SetItem(&item);
            SetItemData(item.row,item.col,profileIter->first);

            ++column;
        }
        
        FUNCTION_EXIT;             
    }

    void IncidentRightsGridControl::CreateInPlaceEditControl(CRect& rect, DWORD dwStyle, UINT nID,
                                                     int nRow, int nCol,
                                                     LPCTSTR szText, int nChar)
    {
        CStringArray accessTypes;

        for( std::map<EAccessType, CString>::iterator iter = m_accessTypes.begin(); iter != m_accessTypes.end(); ++iter)
        {
            accessTypes.Add(iter->second);
        }

	    // InPlaceList and auto-deletes itself
	    new CInPlaceList(this, rect,
                         CBS_DROPDOWNLIST,
                         nID,                   // ID of control being created
                         nRow, nCol, 
                         accessTypes, szText, nChar);
    }   

	//populate header and data
	void IncidentRightsGridControl::setAccessMatrix(AccessMatrix& matrix)
	{
		FUNCTION_ENTRY("setAccessMatrix");

		setupAccessTypes();

		m_accessMatrix = matrix;  

		setUpHeaders();
	 
		CProgressCtrl& progress = m_statusBar->displayProgressBar();
		progress.SetStep(1);
		
		progress.OffsetPos(10);
		resetGrid();		
		
		progress.OffsetPos(30);
		if (!m_accessMatrix.empty())
		{
			// Set up the standard item fields so we don't have to do it each time we insert an item
			GV_ITEM item;
            item.mask = GVIF_TEXT|GVIF_FORMAT;
			item.nFormat = DT_LEFT|DT_WORDBREAK;			
			 
			unsigned long accessorKey;

			for (AccessMatrix::iterator matIter = m_accessMatrix.begin(); matIter != m_accessMatrix.end(); ++matIter )
			{
				int matchingRow = 0;                
                // First find the matching row (profile) for this accessing key
                accessorKey = matIter->first;

				unsigned long rowData;
                for ( int row = 1; row < GetRowCount(); ++row)
                {
                    rowData = GetItemData(row,0);
                    if (rowData == accessorKey)
                    {
						matchingRow = row;
                        break;
                    }
                }
				// Now loop into the creator access maps for this accessing profile key	 
				
				unsigned long creatorKey;
				int matchingCol;

				ProfileAccess creatorProfile = matIter->second;
				ProfileAccess::iterator profIter;

				for (profIter = creatorProfile.begin(); profIter != creatorProfile.end(); ++profIter)
				{
					matchingCol = 0;
					
					creatorKey = profIter->first;
					
					// find the column id for this creator profile key
					unsigned long colData;
					for ( int col = 1; col < GetColumnCount(); ++col)
					{
						colData = GetItemData(0,col);
						if (colData == creatorKey)
						{
							matchingCol = col;
							break;
						}
					}
				 
					if (matchingCol != 0 && matchingRow != 0)
					{
						item.col = matchingCol;
						item.row = matchingRow;						
						
						// get the access type string for the creator profile's access type						 
						std::map<EAccessType, CString>::iterator accessType = m_accessTypes.find(profIter->second);
						if (accessType != m_accessTypes.end())
						{
							item.szText = accessType->second;
							SetItem(&item);							 
							SetItemData( matchingRow,matchingCol, profIter->second);
						}						
					}					
				}
				
			}			
		}		

		progress.OffsetPos(10);
		 // Now that we've set up we have to set the font on the control
        CFont arialBold;
        arialBold.CreatePointFont(FONTSIZE_FOR_CONTROLS, TYPEFACE_FOR_CONTROLS);
		SetFont(&arialBold);

		 // Now all the data is inserted we should auto-size the control to the right size for the text
	    AutoSize();

		FUNCTION_EXIT;		
	}
	
	bool IncidentRightsGridControl::hasChanged()
	{
		FUNCTION_ENTRY("hasChanged");
		FUNCTION_EXIT;
		return m_bDataChanged;
	}

	void IncidentRightsGridControl::setStatusBar(TA_Base_App::IProgressStatusBar* statusBar)
	{
		FUNCTION_ENTRY("setStatusBar");		
        TA_ASSERT(statusBar != NULL, "The status bar passed to us was NULL");		
        m_statusBar = statusBar;			
        FUNCTION_EXIT;		
	}


	AccessMatrix IncidentRightsGridControl::getAccessMatrix() 
	{	
		FUNCTION_ENTRY("getAccessMatrix");	
		
		if (m_bDataChanged)
		{		
			// m_accessMatrix.clear();
			
			// clear the previous matrix
			int colSize = GetColumnCount();
			int rowSize = GetRowCount();
			
			ProfileAccess theMap;
			unsigned long accessorId;
			unsigned long creatorId;
			EAccessType accessLevel;
			// bool bHasAccess;		 
			bool bDataHasChanged = false;
			
			for (int row = 1; row < rowSize; ++row )
			{
				accessorId = GetItemData(row,0);
				
				//bHasAccess = false;
				
				theMap = m_accessMatrix[accessorId];

				for (int col = 1; col < colSize; ++col )
				{   
					creatorId = GetItemData(0,col);
					accessLevel = (EAccessType) GetItemData(row,col);	
					//TD15732 - modified algo so that when an incident rights is 
					// changed to NO_CONTROL, database could be updated.
					//if (accessLevel != NO_CONTROL)
					//	bHasAccess = true;					
					//theMap[creatorId] = accessLevel;

					// find if the data is in the current matrix
					ProfileAccess::iterator profIter = theMap.find(creatorId);
					if (profIter != theMap.end())
					{
						if (accessLevel != profIter->second)
						{
							theMap[creatorId] = accessLevel;
							bDataHasChanged = true;
						}
					}
					else if (accessLevel != NO_CONTROL)
					{
						theMap[creatorId] = accessLevel;
						bDataHasChanged = true;
					}					
					//++lizettejl
				}

				// for a certain profile to be added to the access matrix, it must at
				// leat have one access rights to the other profile, otherwise, no need
				// to add in a profile to the matrix that has no create or view access for an
				// incident report
			    //if (bHasAccess && !theMap.empty())
				if (bDataHasChanged)			 
				{
					m_accessMatrix[accessorId] = theMap;
				}
			}			
		}
		FUNCTION_EXIT;
		return m_accessMatrix;
	}

	
	void IncidentRightsGridControl::OnEndEditCell(int nRow, int nCol, CString str)
    {
		
		if (!m_bIsReset)
		{
			bool bNewData = false;
			
			EAccessType newAccessLevel = getAccessLevelKey(str);		 
			
			int accessKey = GetItemData(nRow,0);
			int createKey = GetItemData(0,nCol);
			
			// find if the edited data is in the current matrix
			AccessMatrix::iterator matIter = m_accessMatrix.find(accessKey);
			if (matIter != m_accessMatrix.end())
			{
				ProfileAccess profAccess= matIter->second;
				ProfileAccess::iterator profIter = profAccess.find(createKey);
				
				if (profIter != profAccess.end())
				{
					if (newAccessLevel != profIter->second)
					{
						bNewData = true;
					}
				}
			}

			// edited cell is not in the matrix, check if a new access type is being set
			// aside from NO_CONTROL			
			if(!bNewData)
			{
				if (!str.IsEmpty())
					bNewData = true;
			}
			
			if (bNewData)
			{			  
				SetItemData(nRow,nCol,newAccessLevel);
				m_bDataChanged = true;
				IncidentRightsView* mainView = dynamic_cast<IncidentRightsView*>(GetParent());
				if (mainView != NULL)
				{
					// We must now enable the reset and apply button since the user has made a change
					mainView->refreshButtonAvailability(true);
				}			 
			}
		}
		CGridCtrl::OnEndEditCell(nRow, nCol, str);
        //FUNCTION_EXIT;
    }
 
	std::map<unsigned long, std::string> IncidentRightsGridControl::getAllProfileGroups()
    {
        FUNCTION_ENTRY("getAllProfileGroups");

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the profile groups from the database
        std::vector<TA_Base_Core::IProfileGroup*> profileGroupsData;
        try
        {
            profileGroupsData = TA_Base_Core::ProfileGroupAccessFactory::getInstance().getAllProfileGroups();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No profile group will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No profile group will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map 
        // 2) Delete the IProfileGroup object when we're done
		std::vector<TA_Base_Core::IProfileGroup*>::iterator iter;
        for (iter = profileGroupsData.begin(); iter != profileGroupsData.end(); ++iter)
        {
			try
            {
				unsigned long groupKey = (*iter)->getKey();
				std::string groupName = (*iter)->getName();
				namesAndKeys.insert( std::map<unsigned long, std::string>::value_type(groupKey,groupName));           
			}
            catch(const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the profile group name. It will not be displayed to the user");
            }
            catch(const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the profile group name. It will not be displayed to the user");
            }
        }

		//after we are done with the IProfileGroup data, clean up
		for (iter = profileGroupsData.begin(); iter != profileGroupsData.end(); ++iter)
		{
			delete *iter;   
		}

        FUNCTION_EXIT;
        return namesAndKeys;
    }	 

	void IncidentRightsGridControl::resetGrid()
    {
		FUNCTION_ENTRY("resetGrid");	 

		m_bIsReset = true;

        GV_ITEM item;
        item.mask = GVIF_TEXT|GVIF_FORMAT;
        item.nFormat = DT_LEFT|DT_WORDBREAK;

        for (int row = 1; row < GetRowCount(); ++row)
        {
            for ( int col = 1; col < GetColumnCount(); ++col)
            {
                item.szText = "";
                SetItem(&item);
                SetItemData( row,col,0 );
                OnEndEditCell(row, col, "");
            }
        }
		
		m_bIsReset = false;

		FUNCTION_EXIT;
    }
	
	void IncidentRightsGridControl::setupAccessTypes()
	{		
		FUNCTION_ENTRY("setupAccessTypes");
		CString strAccess;		

		m_accessTypes.clear();

		m_accessTypes.insert(pair<EAccessType,CString>(NO_CONTROL,""));
		strAccess.LoadString(IDS_ACCESS_VIEW);
		m_accessTypes.insert(pair<EAccessType,CString>(VIEW,strAccess));
		strAccess.LoadString(IDS_ACCESS_CONTROL_OWN);
		m_accessTypes.insert(pair<EAccessType,CString>(CONTROL_OWN,strAccess));
		strAccess.LoadString(IDS_ACCESS_CONTROL_OCC);
		m_accessTypes.insert(pair<EAccessType,CString>(CONTROL_OCC,strAccess));
		strAccess.LoadString(IDS_ACCESS_CONTROL_ALL);
		m_accessTypes.insert(pair<EAccessType,CString>(CONTROL_ALL,strAccess));
		FUNCTION_EXIT;
	}
	
	EAccessType IncidentRightsGridControl::getAccessLevelKey(const CString &strAccessType)
	{
		FUNCTION_ENTRY("getAccessLevelKey");

		EAccessType accessTypeKey = NO_CONTROL;

		std::map<EAccessType, CString>::iterator accessType;
		for (accessType = m_accessTypes.begin(); accessType != m_accessTypes.end(); ++accessType)
		{
			CString strAccess = accessType->second;
			if (strAccess.Compare(strAccessType) == 0)
			{
				accessTypeKey =	accessType->first;
				break;
			}
		}

		FUNCTION_EXIT;
		return accessTypeKey;
	} 

} 




