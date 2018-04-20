/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/GroupPatchPage.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2010/05/10 17:22:58 $
  * Last modified by:  $Author: builder $
  *
  */
// GroupPatchPage.cpp : implementation file
//

#pragma warning(disable:4786)  // Decorated name truncated

#include <algorithm>

#include "stdafx.h"
#include "resource.h"
#include "RmLayoutHelper.h"
#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"
#include "GroupPatchPage.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/Timer.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "core/message/types/RadioComms_MessageTypes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_RGLEN_GROUPNAME 256
#define MAX_RGDBLEN_PATCHNAME	32
#define MAX_RGDBLEN_RASUBSID 32
#define MAX_RGGROUP_INPATCH 3  // This defines how many groups can be in a patch - TD15105
#define MAX_RGSQL_BUF	2000

#define WM_SUBSCRIBER_UPDATE ( WM_APP + 1 )
#define WM_ADD_ITEM_TO_PATCH_GROUP ( WM_USER + 100 ) // limin++ CL-21358



IMPLEMENT_DYNCREATE(CGroupPatchPage, CDialog)

// MYSQL-FIXME: THIS FILL

/////////////////////////////////////////////////////////////////////////////
// CSearchPage property page

namespace TA_Base_Core
{
    class IDatabase;
    class IData;
}

namespace TA_Base_Bus
{
    class AutoSortListCtrl;
    class AutoSortListCtrl::SortInfo;
}

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::IDatabase;
using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::IData;
using TA_Base_Bus::AutoSortListCtrl;
//using TA_Base_Bus::AutoSortListCtrl::SortInfo;

const int COL_MAX = 2;

CGroupPatchPage::CGroupPatchPage() : m_groupList(COL_MAX), m_patchList(COL_MAX)
{
	//{{AFX_DATA_INIT(CGroupPatchPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//sql = new char[MAX_RGSQL_BUF];   
}

void CGroupPatchPage::ResetCreateState()
{
	m_vlistPatchMem.clear();
    m_vlistPatchTsi.clear();
	m_vRestoreGroup.clear();
	m_vRestoreTSI.clear();
}

void CGroupPatchPage::ResetRemoveState()
{
	m_vRestoreGroup.clear();
	m_vRestoreTSI.clear();
}

void CGroupPatchPage::ResetState()
{
	ResetRemoveState();
	ResetCreateState();
}

/*
void CGroupPatchPage::ClearMemory()
{

   if (sql != NULL)
   {
     delete[] sql;
	 sql = NULL;
   }
}
*/

CGroupPatchPage::~CGroupPatchPage()
{
    unsigned long sz = m_vAttachedData.size();
	
    for (unsigned long l=0; l < sz; l++ )
	{

         delete[] m_vAttachedData[l];  // tried with delete[] will have to look into it
       l++;
	}
//	ClearMemory();
}

void CGroupPatchPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupPatchPage)
	DDX_Control(pDX, IDC_BUTTON3, m_removeButton);
	DDX_Control(pDX, IDC_PATCH, m_patchButton);
	DDX_Control(pDX, IDC_GROUPLIST, m_groupList);
	DDX_Control(pDX, IDC_PATCHLIST, m_patchList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupPatchPage, CDialog)
	//{{AFX_MSG_MAP(CGroupPatchPage)
	ON_BN_CLICKED(IDC_PATCH, OnPatch)
	ON_BN_CLICKED(IDC_BUTTON3, OnRemove)
    ON_MESSAGE(WM_SUBSCRIBER_UPDATE, OnSubscriberNotification)
	ON_MESSAGE(WM_ADD_ITEM_TO_PATCH_GROUP, OnAddItemToPatchGroup) // limin++ CL-21358
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CGroupPatchPage::OnInitDialog() 
{	 
    CDialog::OnInitDialog();

	// Load list of patched groups and owners
	m_patchList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_patchList.InsertColumn(0,"Groups",LVCFMT_LEFT,300);//TDlichao
    
    // TES #552: Change column text to "Done By" 
    // TES #570: Made column wider
	m_patchList.InsertColumn(1,"Done By",LVCFMT_LEFT,100);//TDlichao

	// Load list of groups
	m_groupList.InsertColumn(0,"Group",LVCFMT_LEFT,300); //TDlichao
	m_groupList.InsertColumn(1,"TSI",LVCFMT_LEFT,100);//TD lichao
	
	TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(m_patchList);
	TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(m_groupList);

	initSortInfo();
	
	//TD19766
	EnableToolTips(TRUE);
	m_tip.Create(this);
	m_tip.SetMaxTipWidth(200);
	m_tip.Activate(TRUE);
	pwnd = GetDlgItem(IDC_PATCH);
	m_tip.AddTool(pwnd,"");
	//TD19766
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGroupPatchPage::Init()
{
	onUpdateRights();
	updatePatchGroup();

	// subscribe to RadioDirectoryNotification, in order to sychronize the RadioManager of all the stations.
	try
    {
        m_messageSubscriber = new TA_IRS_Bus::SimpleMessageSubscriber<TA_Base_Bus::IRadioCorbaDef::SubscriberDetailsNotification>(
                                            TA_Base_Core::RadioComms::RadioDirectoryNotification, 
                                            WM_SUBSCRIBER_UPDATE,
                                            getEntityLocationKey());

        m_messageSubscriber->addListenerWindow(m_hWnd, true);
    }
    catch (...)
    {        
        return;
    }
}

//update patch list and the Group list.
void CGroupPatchPage::updatePatchGroup()
{
	int	iItem;	
	if(m_groupList.m_hWnd!=NULL)
	{
		m_groupList.DeleteAllItems();
	}
	if(m_patchList.m_hWnd!=NULL)
	{
		m_patchList.DeleteAllItems();
	}

	// Get the parent class and retrieve the reference to the readio manager
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

    TA_Base_Bus::IRadioCorbaDef::SubscriberList_var groups; 
    TA_Base_Bus::IRadioCorbaDef::SubscriberList_var patches;
    bool retrievedSubscribersOK = false;

    try
    {
        CORBA_CALL_RETURN( groups, manager->getRadioCorbaDef(), getGroupSubscribers, () );//limin
        CORBA_CALL_RETURN( patches, manager->getRadioCorbaDef(), getPatchSubscribers, () );//limin
        retrievedSubscribersOK = true;
    }
    SILENT_RM_CATCH_HANDLER("getGroup/getPatch subscribers");

    if (!retrievedSubscribersOK)
    {
        return;
    } 
    
    // Before continuing need to ensure that no group that is currently part of a patch shows up in the 
    // groups list. To do this obtain a list of all groups currently a member of patches. Can do that while
    // loading patch groups....so....
    // Create a vector in which to store the "blocked" group ITSIs
	std::vector<std::string> patchMembers;

    unsigned int i = 0;
    for (i = 0;i < patches->length();i++)
    {
        char ownerName[MAX_RGLEN_GROUPNAME];
        try
        {
			// TODO - Why are we getting a blank owner?
			if (strlen(patches[i].ownerTSI)>0)
			{

                TA_Base_Bus::IRadioCorbaDef::SubscriberDetails_var patchOwner;
                CORBA_CALL_RETURN( patchOwner, manager->getRadioCorbaDef(), getSubscriberDetails, (patches[i].ownerTSI) );
			}
        }
        SILENT_RM_CATCH_HANDLER("getSubscriberDetails");        
        // If this throws an exception, display the patch group without an owner
        
		// Only add the group to pane if its not found in persisted patches
		std::string patchname =  patches[i].subscriberName;
        iItem = m_patchList.InsertItem(i,patches[i].subscriberName);
		strcpy(ownerName, (GetOwnerITSI(patchname).c_str())) ;
    	m_patchList.SetItemText(iItem,1,ownerName);

		// RASUBS_ID from RA_SUBSCRIBER for patch 
		size_t uSize = MAX_RGDBLEN_RASUBSID + 1;
		char * p = new char[uSize];
		memset(p, 0, uSize);

		strcpy(p, patches[i].TSI);
        m_patchList.SetItemData(iItem, (DWORD)p);  	
		m_vAttachedData.push_back(p);

        // Get the subscribers for this patch group
        try
        {
            TA_Base_Bus::IRadioCorbaDef::SubscriberList_var patchSubscribers;
            CORBA_CALL_RETURN( patchSubscribers, manager->getRadioCorbaDef(), getPatchMembers, (patches[i].TSI) );//limin
    
            // Extract the TSIs of the patch members and add it to the vector
            for (unsigned int j = 0;j < patchSubscribers->length();j++)
            {
                patchMembers.push_back(std::string(patchSubscribers[j].TSI));
            }
        }
        SILENT_RM_CATCH_HANDLER("getPatchMembers");
    }

    for (i = 0;i < groups->length();i++)
	{
		// Work out if we actually want this group - because if it's been used in a patch already, it can't 
		// be used in another patch.
		std::string groupTSI = groups[i].TSI;
		if (std::find(patchMembers.begin(),patchMembers.end(),groupTSI) == patchMembers.end())
		{
			// iItem = m_groupList.InsertItem(i,groups[i].subscriberName);
			// m_groupList.SetItemText(iItem,1,groupTSI.c_str());	                 

            std::string* group = new std::string( groups[i].subscriberName );
            std::string* tsi = new std::string( groupTSI );
            PostMessage( WM_ADD_ITEM_TO_PATCH_GROUP, reinterpret_cast<WPARAM>( group ), reinterpret_cast<LPARAM>( tsi ) );                
		}
	}
}

void CGroupPatchPage::UpdateGroups()
{
	int	iItem;
	int nCount = m_groupList.GetItemCount();

	for (unsigned long i =0 ; i < m_vRestoreGroup.size(); i++)
	{
	    // Item = m_groupList.InsertItem(nCount + 1, m_vRestoreGroup[i].c_str());
		// m_groupList.SetItemText(iItem,1, m_vRestoreTSI[i].c_str());	 
	
        std::string* group = new std::string( m_vRestoreGroup[i] );
        std::string* tsi = new std::string( m_vRestoreTSI[i] );
        PostMessage( WM_ADD_ITEM_TO_PATCH_GROUP, reinterpret_cast<WPARAM>( group ), reinterpret_cast<LPARAM>( tsi ) );
	}
	ResetState();
}


std::string CGroupPatchPage::GetOwnerITSI(std::string patchname)
{
	    int j=0;
        char sql[MAX_RGSQL_BUF] = {0};
		
		  j =  sprintf(sql,      "%s", "SELECT OWNER FROM RA_SUBSCRIBERS  where SUBSCRIBER_NAME = '"); 
		  j += sprintf(sql + j, "%s",  patchname.c_str());
		  j += sprintf(sql + j, "%s",  "'");
		

		// Set up the columns

		std::string Owner    = "OWNER";

        std::vector<std::string> columnNames;

        columnNames.push_back(Owner);


		// Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it
        // We use this call direct, as we *want* multiple values returned
        // get a connection to the database
       IDatabase* db =  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
		

        IData* data = db->executeQuery( sql, columnNames );

        if (0 == data->getNumRows()) // No entry found 
        {
            delete data;
            data = NULL;
			return NULL;
        }
	

		  // More than one entry found for the pkey
        else if (1 < data->getNumRows())
        {
            std::ostringstream reasonMessage;
            reasonMessage << "More than one patch found with patch name as " <<  patchname.c_str();

			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = patchname.c_str();
			userMsg << fieldName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632009);
			/*MessageBox((reasonMessage.str()).c_str(), "Warning");*/
			MessageBox(errMsg, "Warning");
			// ++ TD14164 

             // clean up the pointer
            delete data;
            data = NULL;
			//delete db;

			TA_THROW( DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "") );
         }
		 else
		 {

              // Assign the data as appropriate to the member variables.
 		     std::string owner = data->getStringData(0, Owner);

            // State has to be maintained so do not call ResetState()
            delete data;
            data = NULL;
			//delete db;

    		return owner;
		 }
		
}

void CGroupPatchPage::onUpdateRights()
{
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

	bool enable = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_CONFIGURE_PATCH_GROUPS);

	m_patchButton.EnableWindow(enable && hasValidPatchSelection());
	m_removeButton.EnableWindow(enable && (m_patchList.GetSelectedCount() > 0));
}


bool CGroupPatchPage::hasValidPatchSelection() const
{
    const DWORD GROUP_SIZE_MIN = 2;
    const DWORD GROUP_SIZE_MAX = MAX_RGGROUP_INPATCH;
    
    DWORD selectedCount = m_groupList.GetSelectedCount();

    return selectedCount >= GROUP_SIZE_MIN && selectedCount <= GROUP_SIZE_MAX;
}


afx_msg void CGroupPatchPage::OnCancel()
{
	// Do nothing
}

afx_msg void CGroupPatchPage::OnOK()
{
	// Do nothing
}

BOOL CGroupPatchPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    FUNCTION_ENTRY("OnNotify");

    // Get at the message header to determine the type of message we've received
    LPNMHDR lpnm  = ((LPNMHDR)lParam);

    
    if(lpnm->code == NM_CLICK)
    {
        // IF this is a click notification, cast the message to an ITEMACTIVATE struct
        LPNMITEMACTIVATE pClick = (LPNMITEMACTIVATE)lParam;

        // Need to know which of the two list controls sent the message
        if ((lpnm->idFrom == IDC_GROUPLIST) ||
			(lpnm->idFrom == IDC_PATCHLIST))
        {        
            onUpdateRights();
        }
    }

    FUNCTION_EXIT;
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CGroupPatchPage::OnPatch() 
{

	bool patchinDB = FALSE;
	std::string ownerProfileName;
	CGroupPatchPage::GetCurrentTime();

    // Just make sure that enough groups have been selected...
    if (!hasValidPatchSelection())
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString errMsg = userMsg.constructMessage(IDS_UE_632006);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            "Either two or three groups must be selected before a patch can be applied",
            "Patch Operation Error",
            MB_ICONERROR | MB_OK);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Patch Operation Error",
            MB_ICONERROR | MB_OK);
		// ++ TD14164 
    }
    else
    {
        // Get the parent class and retrieve the reference to the readio manager
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

        // Determine the groups that have been patched - to do this get a reference to the position of the first item
        POSITION selPos = m_groupList.GetFirstSelectedItemPosition();

        // Prepare a string to hold the names to make a label for the patch
        std::string patchNames;
		std::string TSINumbers;
        // And a vector to put the indicies of the patch groups in
        std::vector<int> listIndexs;


        // And prepare a PatchList into which to insert the items.
        TA_Base_Bus::IRadioSessionCorbaDef::PatchList patchList;
        patchList.length(m_groupList.GetSelectedCount());   // Sequences need to be told how long they are

        unsigned int patchNumber = 0;
		std::string itsi;
        do
        {   
		
            // Get the actual index of the selected items
            int index = m_groupList.GetNextSelectedItem(selPos);

            listIndexs.push_back(index);
			m_vlistPatchMem.push_back(m_groupList.GetItemText(index,0).GetBuffer(0));
            itsi = m_groupList.GetItemText(index, 1).GetBuffer(0);
			m_vlistPatchTsi.push_back(itsi);

            // Insert the string in the first column into patch names
            patchNames += m_groupList.GetItemText(index,0);
			TSINumbers += itsi.substr(0,itsi.find_first_of('-'));
			if (patchNames.length() > MAX_RGDBLEN_PATCHNAME)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
				 "The length of Patch name is more than 32");
		        std::ostringstream reasonMessage;
                reasonMessage  << "Patch name is more than "  << MAX_RGDBLEN_PATCHNAME <<"characters, we will use the TSI as the patch name!" ;
                //TD19766
				/*
					// TD14164 ++
					TA_Base_Bus::TransActiveMessage userMsg;
					CString errMsg = userMsg.constructMessage(IDS_UW_632010);
					//MessageBox((reasonMessage.str()).c_str(), "CHECK");
					MessageBox(errMsg, "CHECK");
					// ++ TD14164 
				*/
				m_tip.UpdateTipText(reasonMessage.str().c_str(),pwnd);
				//return;
				//TD19766
				patchNames = TSINumbers;
			}

			if (m_vlistPatchMem.size() > MAX_RGGROUP_INPATCH) 
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
				 "Cannot create a patch with more than three static groups");
		        std::ostringstream reasonMessage;
                reasonMessage << "Cannot create a patch with more than "  << MAX_RGGROUP_INPATCH << " static groups" ;
                
				// TD14164 ++
				TA_Base_Bus::TransActiveMessage userMsg;
				std::ostringstream str;
				str << MAX_RGGROUP_INPATCH;
				std::string number = str.str();;
				userMsg << number.c_str();
				CString errMsg = userMsg.constructMessage(IDS_UW_632011);
				/*MessageBox((reasonMessage.str()).c_str(), "CHECK");*/
				MessageBox(errMsg, "CHECK");
				// ++ TD14164 

				ResetState();
				return;

			}
            if (selPos != NULL)
            {
                // For all but the last item added, insert to padding and the "+" character
                patchNames += " + ";
				TSINumbers += " + ";
            }

            // And insert the string in the second column at that index into the patch (it's the itsi)
            patchList[patchNumber] = m_groupList.GetItemText(index,1);
            patchNumber++;
        }
        while (selPos != NULL);

        // Now that we have our patch list, can call CreatePatch
        try
        {		
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				 "OnPatch::createPatchtoCorba,PatchNames=%s",patchNames.c_str());
            CORBA_CALL( manager->getRadioSessionCorbaDef(), createPatch, ( patchList,patchNames.c_str(),
				        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				 "OnPatch::createPatchtoCorbaComplete");

			// get Owner ITSI for the purposes of authorising a remove patch later
			ownerProfileName = GetLocalProfileName();
			patchinDB = insertPatchinDB(patchNames, ownerProfileName);
		}

        SILENT_RM_CATCH_HANDLER("createPatch");

		if (patchinDB)
		{
           std::sort(listIndexs.begin(),listIndexs.end());
          // Now loop again through the selected items and remove them from the groups list
          for(unsigned int i = 0;i < listIndexs.size();i++)
		  {
              m_groupList.DeleteItem(listIndexs[i] - i); // take i from the index as the actual index changes after each item is deleted
		  }

          // Need to update the patch list to add the new patch.
           int index = m_patchList.InsertItem(m_patchList.GetItemCount(), patchNames.c_str());
           m_patchList.SetItemText(index, 1, ownerProfileName.c_str());

			size_t uSize = MAX_RGDBLEN_RASUBSID + 1;
			char * p = new char[uSize];
			memset(p, 0, uSize);
			strcpy(p, timestamp);

		   m_patchList.SetItemData(index, (DWORD)p);
		   m_vAttachedData.push_back(p);
          // Finally, disable the patch button
          m_patchButton.EnableWindow(FALSE);
		}
    }           
    ResetState();
}


void CGroupPatchPage::GetCurrentTime()
{

	   struct tm *newtime;
	   time_t ltime;

	   time( &ltime );

   /* Obtain coordinated universal time: */
   newtime = gmtime( &ltime );
    
   sprintf(timestamp, "%s", asctime( newtime ) );
}

//TD18317, hongzhi
std::string CGroupPatchPage::getLocalDatabaseName()
{
	std::string localDatabaseName;    
	if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
	{
		throw DatabaseException("db-connection-file not set");
	}
	try
	{
		DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
		DataConnections connections = connectionStrings.getConnectionList(Pa_Cd, Write);
		localDatabaseName = connections[0].first;  
	}
	catch(...) // most likely a file not found error
	{
		throw DatabaseException("Unable to find database connection infomation");
	}
	return localDatabaseName;
}

bool CGroupPatchPage::insertPatchinDB(std::string displayName, std::string profileName)
{
	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
		 "InsertPatchinDB()::displayName=%s,profileName=%s",displayName.c_str(),profileName.c_str());

	IDatabase* db = 
              TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
	
	unsigned int patchsize = m_vlistPatchTsi.size();
    char  dbname[MAX_RGDBLEN_PATCHNAME + 1];
	
	strncpy(dbname, displayName.c_str(), MAX_RGDBLEN_PATCHNAME);
	dbname[MAX_RGDBLEN_PATCHNAME] = '\0';
	//TD18317, zhanghongzhi
	std::stringstream sql1;
	std::string str_timestamp(timestamp);
	sql1 << "BEGIN "
        << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'PUBLIC', '"
	    << "INSERT INTO RA_SUBSCRIBERS ( RASUBS_ID, SUBSCRIBER_NAME, LOCATION, SUBSCRIBER_TYPE, LOCATION_KEY, OWNER) VALUES "
	    << "(''" << str_timestamp << "'', ''" << displayName <<"'', ''OCC'', ''P'', 1, ''"	
		<< profileName
	    << "'')'); END;";
	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
		"sql1=%s", sql1.str().c_str());

	try
	{
		db->executeModification(sql1.str().c_str());
	}
	catch (...)
	{
		// Did not make it - possibly statistic key not defined
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
			 "Unable to save patch on the database. RA_SUBSCRIBER");
		ResetState();
		//delete db;
		return FALSE;

	}  
	
	// Build the insert string	
	for (unsigned int k=0; k < patchsize; k++)
    {
		//TD18317, zhanghongzhi
		std::stringstream sql2;
		sql2 << "BEGIN "
			<< "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'PUBLIC', '"
			<< "INSERT INTO RA_PATCH_MEMBERS ( PATCH_RASUBS_ID, GROUP_RASUBS_ID ) VALUES "
			<< "(''" << str_timestamp << "'', ''" << m_vlistPatchTsi[k]	<< "'')'); END;";

		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			"sql2=%s", sql2.str().c_str());

		try
		{
			db->executeModification(sql2.str().c_str());
		}

		catch (...)
		{
			// Did not make it - possibly statistic key not defined
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
				 "Unable to save patch on the database. RA_PATCHMEMBERS");
			ResetState();
			//delete db;

			return FALSE;
		}
	}

    ResetState();
	//delete db;

	return TRUE;
}

bool CGroupPatchPage::CanRemove(std::string patchrasubs) 
{
	std::string ownerProfileName;
	unsigned long id;
    int j = 0;

    // create the SQL string to retrieve the owners itsi for the groups

	char sql[MAX_RGSQL_BUF] = {0};
		j  = sprintf(sql,      "%s", "SELECT p.RAPMEM_ID, p.PATCH_RASUBS_ID, p.GROUP_RASUBS_ID,  s.OWNER");
		j += sprintf(sql + j,  "%s", " FROM RA_PATCH_MEMBERS p join  RA_SUBSCRIBERS  s  ON  (s.RASUBS_ID = p.PATCH_RASUBS_ID)");		
		j += sprintf(sql + j,  "%s", " where RASUBS_ID = '");
		j += sprintf(sql + j,  "%s",  patchrasubs.c_str());
		j += sprintf(sql +j,   "%s",  "'");
		
		// Set up the columns
		std::string IdColumn = "RAPMEM_ID";
        std::string PatchRasubsColumn = "PATCH_RASUBS_ID";
        std::string GroupRasubsColumn = "GROUP_RASUBS_ID";
		std::string OwneritsiColumn = "OWNER";

        std::vector<std::string> columnNames;

        columnNames.push_back(IdColumn);
        columnNames.push_back(PatchRasubsColumn);
        columnNames.push_back(GroupRasubsColumn);
		columnNames.push_back(OwneritsiColumn);


		// Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it
        // We use this call direct, as we *want* multiple values returned
        // get a connection to the database
        IDatabase* db =  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
		

        IData* data = db->executeQuery( sql, columnNames );



         // Assign the data as appropriate to the member variables.
            id = data->getUnsignedLongData(0, IdColumn);
            ownerProfileName = data->getStringData(0, OwneritsiColumn);

			// Now get the owner itsi
          // Get the parent class and retrieve the reference to the readio manager
            CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
			try 
			{
				std::string localProfileName = GetLocalProfileName();
      			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
	 			 "Owner of %s is %s. LocalProfile=%s",patchrasubs.c_str(),ownerProfileName.c_str(),localProfileName.c_str()); 
			   if (ownerProfileName == localProfileName)
			   {
				   // State is to be maintained here so ResetState() is avoided purposely
		    		delete data;
					data = NULL; 
					//delete db;

					return TRUE;
			   }
            }
			catch(...)
			{
			// Did not make it - possibly statistic key not defined
      			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
	 			 "Unable to get Owner ITSI from the Radio Agent and the Radio Hardware");         
				
				ResetState();
				delete data;
				data = NULL;
				//delete db;
                return FALSE; 

			}

		delete data;
		data = NULL; 
		//delete db;

        return FALSE;
}

bool CGroupPatchPage::DeletePatchFromDB(std::string patchrasubs)
{
	
	IDatabase* db = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
	unsigned int j=0;	
	std::stringstream sql1;
	try 
	{              
		sql1 << "SELECT  s.SUBSCRIBER_NAME, p.GROUP_RASUBS_ID"
			<< " FROM RA_PATCH_MEMBERS p join  RA_SUBSCRIBERS  s"
			<< " ON  (s.RASUBS_ID = p.PATCH_RASUBS_ID)"
			<< " where RASUBS_ID = '" << patchrasubs << "'";
		
		// Set up the columns
		std::string SubscriberNameColumn = "SUBSCRIBER_NAME";
		std::string GroupRasubsColumn = "GROUP_RASUBS_ID";
		
		std::vector<std::string> columnNames;
		std::vector<std::string> columnNames2;
		
		columnNames.push_back(SubscriberNameColumn);
		columnNames.push_back(GroupRasubsColumn);
		
		IData* data =  db->executeQuery( sql1.str().c_str(), columnNames );
		// Loop for each row returned
		for (unsigned long i = 0; i < data->getNumRows(); i++)
		{
			
			m_vRestoreTSI.push_back(data->getStringData(i, GroupRasubsColumn));
			
		}
		
		columnNames2.push_back(SubscriberNameColumn);
		
		for (unsigned long l = 0; l < m_vRestoreTSI.size(); l++)
		{
			std::stringstream sql2;
			sql2 << "SELECT  SUBSCRIBER_NAME "
				<< " FROM RA_SUBSCRIBERS "
				<< " where RASUBS_ID = '"
				<< m_vRestoreTSI[l]	<< "'";
			data =  db->executeQuery( sql2.str().c_str(), columnNames2 );
			// Loop for each row returned
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				m_vRestoreGroup.push_back(data->getStringData(i, SubscriberNameColumn));
				
			}
			
		}
		//TD18317, zhanghongzhi
		std::stringstream sql3;
		sql3 << "BEGIN "
			<< "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'PUBLIC', '"
			<< "DELETE FROM RA_PATCH_MEMBERS where PATCH_RASUBS_ID = ''"
			<< patchrasubs << "'''); END;";
		
		db->executeModification(sql3.str().c_str());
		delete data;
		data = NULL;
		//delete db;	
	}
	catch(...)
	{
		ResetState();
		
		//delete db;
		
		// Did not make it - possibly statistic key not defined
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
			"Unable to delete patch from the database. RA_PATCH_MEMBERS");
		
		return FALSE;
		
	}
	
	//TD18317, zhanghongzhi
	std::stringstream sql4;
	sql4 << "BEGIN "
		<< "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'PUBLIC', '"
		<< "Delete from RA_SUBSCRIBERS where RASUBS_ID = ''"
		<< patchrasubs << "'''); END;";
	
	try 
	{
		db->executeModification(sql4.str().c_str());
	}
	catch(...)
	{
		// Did not make it - possibly statistic key not defined
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
			"Unable to delete patch from the database. RA_SUBSCRIBER");
		ResetState();		
		
		//delete db;
		
		return FALSE;
	}
	
	   //delete db;
	
	   return TRUE;
}


void CGroupPatchPage::OnRemove() 
{
    // This is a single selection list, so no need to look for more than the one selection
    // Get the patch name of the selected patch
	
    std::string patchName = m_patchList.GetItemText(m_patchList.GetSelectionMark(),0);
	int index = m_patchList.GetSelectionMark();
    int  pptr;
	pptr = m_patchList.GetItemData(index); 
    char *  p = reinterpret_cast<char* >(pptr);
	std::string patchrasubs(p);

	if (!CanRemove(patchrasubs))
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
		 "The patch can only be deleted from the database by the user who created it");
		std::ostringstream reasonMessage;
        reasonMessage << "The patch can only be deleted from the database by the user who created it" ;
        
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString errMsg = userMsg.constructMessage(IDS_UW_632012);
		/*MessageBox((reasonMessage.str()).c_str(), "CHECK");*/
		MessageBox(errMsg, "CHECK");
		// ++ TD14164 	

        ResetState(); 

		//delete db;	
		
		return ;
	}

//		if (DeletePatchFromDB(patchrasubs)==TRUE)
//		{

		// Get the parent class and retrieve the reference to the readio manager
		CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

		// Delete the patch
		try
		{
            CORBA_CALL( manager->getRadioSessionCorbaDef(), deletePatch, ( patchName.c_str(),
				        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An error occurred while attempting to remove the patch group: %s .",patchName.c_str());
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString errMsg = userMsg.constructMessage(IDS_UW_632013);
			/*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
			"An error occurred while attempting to remove the patch group. Please try again.",
			TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
			MB_OK | MB_ICONEXCLAMATION);*/
			RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
				errMsg,
				TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
				MB_OK | MB_ICONEXCLAMATION);
			// ++ TD14164 

			// Return here, because we don't want to do the rest...
			return;
		}

		//delete from DB
		DeletePatchFromDB(patchrasubs);
		// Remove the patch from the list
		m_patchList.DeleteItem(m_patchList.GetSelectionMark());


		// And disable the patch button
		m_patchButton.EnableWindow(FALSE);

		UpdateGroups();
//	}

    ResetState();
}

std::string CGroupPatchPage::GetLocalProfileName() 
{
	std::string profileName;
	std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_ASSERT(sessionID.size()>0,"No sessionID obtained from run parameters.");

	TA_Base_Core::IProfile*  profile = NULL;

	try
	{
		TA_Base_Bus::AuthenticationLibrary  authLib(true);

		TA_Base_Bus::SessionInfo sessionInfo = authLib.getSessionInfo(sessionID,sessionID);

		profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(sessionInfo.ProfileId[0]);
		profileName = profile->getName();
		delete profile;
		profile = NULL;
	}
	catch (...)
	{
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "determine session profile and location";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632006);
		/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Unable to determine session profile and location.");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
		delete profile;
		profile = NULL;
	}
	return profileName;
}

void CGroupPatchPage::OnDestroy()
{
    try
    {
        if (0 != m_messageSubscriber)
        {
            m_messageSubscriber->removeListenerWindow(m_hWnd);

            delete m_messageSubscriber;
            m_messageSubscriber = 0;
        }

    }
    catch (...)
    {
        LOG(SourceInfo, 
            DebugUtil::ExceptionCatch, 
            "Unknown", 
            "While processing CSearchPage::OnDestroy");
    }
}

LRESULT CGroupPatchPage::OnSubscriberNotification(UINT wParam, LONG lParam)
{
    if (0 == m_messageSubscriber ||
        !m_messageSubscriber->isConnectedListenerWindow(m_hWnd))
    {
        // Must be closing down, ignore message as subscriber won't have it
        // any longer if we're no longer connected
        return 0;
    }

    try
    {  
        const SubscriberNotification update = m_messageSubscriber->getMessageData(wParam, m_hWnd);            
        processUpdate(update);        
    }
    catch (...)
    {      
        LOG(SourceInfo, 
            DebugUtil::ExceptionCatch, 
            "Unknown", 
            "While processing CSearchPage::OnSubscriberNotification");  
    }

    return 0;
}

// limin++ CL-21358
LRESULT CGroupPatchPage::OnAddItemToPatchGroup(UINT wParam, LONG lParam)
{
    std::string& group = *reinterpret_cast<std::string*>( wParam );
    std::string& tsi = *reinterpret_cast<std::string*>( lParam );

    int iItem = m_groupList.InsertItem( m_groupList.GetItemCount(), group.c_str() );  // limin++ CL-21358
    m_groupList.SetItemText( iItem, 1, tsi.c_str() );
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Insert item to patch group [group=%s][tsi=%s]", group.c_str(), tsi.c_str() );

    delete &group;
    delete &tsi;

    return 0;
}
// ++limin CL-21358


void CGroupPatchPage::processUpdate(const SubscriberNotification& update)
{
    switch (update.notifyType)
    {
    case TA_Base_Bus::IRadioCorbaDef::SCAN_SUBSCRIBERS:
        break;
    case TA_Base_Bus::IRadioCorbaDef::NEW_SUBSCRIBER:
        
        switch (update.details.type)
        {
        case TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER:    // FALLTHROUGH               
        case TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER:
            break;
        case TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER:
			updatePatchGroup();
            break;
        default:
            ASSERT(FALSE);                
        }        

        break;

    case TA_Base_Bus::IRadioCorbaDef::UPDATE_SUBSCRIBER:

        switch (update.details.type)
        {
        case TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER:    // FALLTHROUGH               
        case TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER:
            break;
        case TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER:
			updatePatchGroup();
            break;
        default:
            ASSERT(FALSE);                
        }        
        
        break;

    case TA_Base_Bus::IRadioCorbaDef::DELETE_SUBSCRIBER:
        
        switch (update.details.type)
        {
        case TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER:    // FALLTHROUGH               
        case TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER:
            break;
        case TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER:
			if(findPatchIndexByTsi(update.details.TSI.in()) >= 0)
				if (DeletePatchFromDB(update.details.TSI.in())==TRUE)
				{
					// Remove the patch from the list
					m_patchList.DeleteItem(m_patchList.GetSelectionMark());
					// And disable the patch button
					m_patchButton.EnableWindow(FALSE);
					UpdateGroups();
				}
			break;
        default:
            ASSERT(FALSE);                
        }        
        
        break;

    default:
        ASSERT(FALSE);
    }
    
	ResetState();
    // Control doesn't always refresh fully, so help it along..
    Invalidate(FALSE);
}

DWORD CGroupPatchPage::getEntityLocationKey()
{
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
    return manager->getLocationKey();
}

//return index of the patch list; -1 if not found.
int CGroupPatchPage::findPatchIndexByTsi(std::string tsi)
{
	for(int i = 0; i<m_patchList.GetItemCount(); i++)
	{
		int pptr = m_patchList.GetItemData(i); 
		char * p = reinterpret_cast<char* >(pptr);
		std::string patchrasubs(p);
		if(tsi == patchrasubs)
		{
			return i;
		}
	}

	return -1;
}

BOOL CGroupPatchPage::PreTranslateMessage(MSG* pMsg) //TD19766
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_tip.m_hWnd!=NULL)  
		m_tip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CGroupPatchPage::initSortInfo()
{
	TA_Base_Bus::AutoSortListCtrl::SortInfo groupSortInfo(COL_MAX);
	TA_Base_Bus::AutoSortListCtrl::SortInfo patchSortInfo(COL_MAX);

	groupSortInfo.defineSortingSemantic(0, TA_Base_Bus::AutoSortListCtrl::BY_STRING);
    groupSortInfo.defineSortingSemantic(1, TA_Base_Bus::AutoSortListCtrl::BY_STRING);
	groupSortInfo.setCurrentSort(0, TA_Base_Bus::AutoSortListCtrl::ASCENDING);
	groupSortInfo.activateSort(0);
	m_groupList.setSortInfo(groupSortInfo);

	patchSortInfo.defineSortingSemantic(0, TA_Base_Bus::AutoSortListCtrl::BY_STRING);
    patchSortInfo.defineSortingSemantic(1, TA_Base_Bus::AutoSortListCtrl::BY_STRING);
	patchSortInfo.setCurrentSort(0, TA_Base_Bus::AutoSortListCtrl::ASCENDING);
	patchSortInfo.activateSort(0);
	m_patchList.setSortInfo(patchSortInfo);
}
