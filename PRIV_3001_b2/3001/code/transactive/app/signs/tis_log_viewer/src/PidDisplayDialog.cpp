/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/PidDisplayDialog.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Used to display destination PIDs.
  *
  */

#include "stdafx.h"
#include "TisLogViewer.h"
#include "PidDisplayDialog.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

// TES885 ~ added
#include "boost/tokenizer.hpp"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <sstream>

namespace TA_IRS_App
{

/////////////////////////////////////////////////////////////////////////////
// PidDisplayDialog dialog


PidDisplayDialog::PidDisplayDialog(CWnd* pParent /*=NULL*/)
	: CDialog(PidDisplayDialog::IDD, pParent),
      m_pidVector()
{
	//{{AFX_DATA_INIT(PidDisplayDialog)
	m_destination = _T("");
	//}}AFX_DATA_INIT
}


void PidDisplayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PidDisplayDialog)
	DDX_Control(pDX, IDC_PID_LIST, m_pidList);
	DDX_Text(pDX, IDC_DESTINATION, m_destination);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PidDisplayDialog, CDialog)
	//{{AFX_MSG_MAP(PidDisplayDialog)
	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_EN_SETFOCUS(IDC_DESTINATION, OnSetfocusDestination)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PidDisplayDialog message handlers


BOOL PidDisplayDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // populate the list
    m_pidList.ResetContent();
    for (std::vector<std::string>::iterator iter = m_pidVector.begin();
    iter != m_pidVector.end(); iter++)
    {
        m_pidList.InsertString(-1, iter->c_str());
    }
    m_pidVector.clear();
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void PidDisplayDialog::displayTtisDestinations( const std::string& destinationTrain,
                                                const std::string& tTisDestinationPids)
{
    // set the destination
    m_destination.Format("Train %s", destinationTrain.c_str());

    // used to break up space delimited strings of numbers
    // add the space to work around eof being triggered on the last valid read
    std::stringstream delimiterStream;
    delimiterStream << tTisDestinationPids << " ";

    int temp = -1;

    delimiterStream >> std::skipws >> temp;
    while ( (temp != -1) && (!delimiterStream.eof()) )
    {
        switch (temp)
        {
    //defectID(17926) AVteam++    
		case TA_Base_Core::ENTIRE_TRAIN:
            m_pidVector.push_back(std::string("Entire Train"));
            break;
        case TA_Base_Core::ALL_CAR1:
            m_pidVector.push_back(std::string("Entire Car 1"));
            break;
        case TA_Base_Core::ALL_CAR2:
            m_pidVector.push_back(std::string("Entire Car 2"));
			break;
        case TA_Base_Core::ALL_CAR3:
            m_pidVector.push_back(std::string("Entire Car 3"));
     //defectID(17926) ++AVteam        
			break;
        case TA_Base_Core::CAR1_PID1:
            m_pidVector.push_back(std::string("Car 1 PID 1"));
            break;
        case TA_Base_Core::CAR1_PID2:
            m_pidVector.push_back(std::string("Car 1 PID 2"));
            break;
        case TA_Base_Core::CAR1_PID3:
            m_pidVector.push_back(std::string("Car 1 PID 3"));
            break;
        case TA_Base_Core::CAR1_PID4:
            m_pidVector.push_back(std::string("Car 1 PID 4"));
            break;
        case TA_Base_Core::CAR2_PID1:
            m_pidVector.push_back(std::string("Car 2 PID 1"));
            break;
        case TA_Base_Core::CAR2_PID2:
            m_pidVector.push_back(std::string("Car 2 PID 2"));
            break;
        case TA_Base_Core::CAR2_PID3:
            m_pidVector.push_back(std::string("Car 2 PID 3"));
            break;
        case TA_Base_Core::CAR2_PID4:
            m_pidVector.push_back(std::string("Car 2 PID 4"));
            break;
        case TA_Base_Core::CAR3_PID1:
            m_pidVector.push_back(std::string("Car 3 PID 1"));
            break;
        case TA_Base_Core::CAR3_PID2:
            m_pidVector.push_back(std::string("Car 3 PID 2"));
            break;
        case TA_Base_Core::CAR3_PID3:
            m_pidVector.push_back(std::string("Car 3 PID 3"));
            break;
        case TA_Base_Core::CAR3_PID4:
            m_pidVector.push_back(std::string("Car 3 PID 4"));
            break;
        default:
            break;
        }

        temp = -1;
        delimiterStream >> std::skipws >> temp;
    }
    

    // pop up the dialog
    DoModal();
}

        

void PidDisplayDialog::displayStisDestinations(const std::string& sTisDestinationStation,
                                               const std::string& sTisDestinationLevels,
                                               const std::string& sTisDestinationPids)
{
    // set the destination
    m_destination.Format("Station %s", sTisDestinationStation.c_str());

    // If there were no PID's then the entire station was selected
    // TD8387 fix.
    if( 0 == sTisDestinationPids.size() )
    {        
        // TES885 ~ replace "All PIDs" with actual list of PIDs
        m_pidVector = getAllStationPIDs(sTisDestinationStation);
        // m_pidVector.push_back( std::string( "All PID's" ) );
    }
    // There is a list of destination PID's
    else
    {
        // used to break up space delimited strings of numbers
        // add the space to work around eof being triggered on the last valid read
        std::stringstream delimiterStream;
        delimiterStream << sTisDestinationLevels << " "
            << sTisDestinationPids << " ";

        std::string temp("");

        delimiterStream >> std::skipws >> temp;
        while ( (temp.length() > 0) && (!delimiterStream.eof()) )
        {
            m_pidVector.push_back(temp);

            temp = "";
            delimiterStream >> std::skipws >> temp;
        }
    }

    // pop up the dialog
    DoModal();
}


void PidDisplayDialog::OnSetfocusDestination() 
{
    // immediately give the focus to the button
	GetDlgItem(IDCLOSE)->SetFocus();
}


void PidDisplayDialog::OnClose() 
{
	CDialog::OnCancel();
}

// TES885 ~ added
std::vector<std::string> PidDisplayDialog::getAllStationPIDs(const std::string& theStation)
{
    TA_ASSERT(theStation.length(), "Station name undefined.");

    // search on STN.TIS.* for the specified station
    std::string searchToken(".TIS.%");
    searchToken = theStation + searchToken;

    std::vector<std::string> pidNames;

    try
    {    
        std::vector<TA_Base_Core::IEntityData*> entityData;
        entityData = TA_Base_Core::EntityAccessFactory::getInstance().
            getEntitiesOfTypeWithNameLikeToken(
                TA_Base_Core::DataNodeEntityData::getStaticType(),
                searchToken);

        // convert the EntityData to pid names        
        for (std::vector<TA_Base_Core::IEntityData*>::iterator 
             i = entityData.begin(); i != entityData.end(); ++i)
        {
            // break name on '.'
            std::vector<std::string> parts = tokenizePIDName((*i)->getName());

            // from the start to the first '.' is the station name
            // from the first '.' to the second '.' is TIS
            // from the second '.' to the third '.' is the level
            // from the third '.' to the end of the string is the PID name

            // so a PID will have 4 parts
            if (parts.size() == 4)
            {
                // ignore certain data nodes
                if( (parts[3] != "ISCS") &&
                    (parts[3] != "SEV") &&
                    (parts[3] != "TIS01") && 
                    (parts[3] != "TIS02") )
			    {
				    // this is a valid PID, use its name
                    pidNames.push_back(parts[3]);
			    }
            }
            else
            {
				// invalid PID
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
							"Invalid PID given for selection %s",
							(*i)->getName().c_str() );
                continue;
            }
        }
    }
    catch (...)
    {

    }

    return pidNames;
}

// TES885 ~ added
std::vector<std::string> PidDisplayDialog::tokenizePIDName(std::string pidEntityName)
{
    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    std::vector<std::string> parts;    
    boost::char_separator<char> separator(".");

    tokenizer tokens(pidEntityName, separator);
    
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        parts.push_back(*tok_iter);
    }

    // if parts is empty, then this should return the entire string
    if (parts.size() == 0)
    {
        parts.push_back(pidEntityName);
    }

    return parts;
}


} // TA_App

