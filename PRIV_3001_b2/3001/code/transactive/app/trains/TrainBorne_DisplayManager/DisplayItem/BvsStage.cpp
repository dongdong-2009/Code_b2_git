/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_display_manager/src/BvsStage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */

#include "TrainBorne_DisplayManager/stdafx.h"

#include "BvsStage.h"
//#include "VideoDisplayManagerDlg.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus\generic_gui\src\TransActiveMessage.h" //TD20501
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>


using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

using TA_IRS_Bus::LocationCache;

namespace TA_IRS_App
{

    typedef BvsStage::TrainVideoSourceType TrainVideoSourceType;

    BvsStage::BvsStage( unsigned long entityKey,
                        unsigned long stageId,
                        const std::string& name,
                        const std::string& description,
                        const std::string& group,
                        std::set<unsigned long> locations )
      : VideoInput(entityKey, name, description, group),
        m_stageId( stageId ),
        m_locations(locations),
        m_locationDescription(""),
        m_activeTrain(0),
        m_preConflict(false),
        m_activeVideoSource(None),
        m_numActiveMonitors(0)
	
    {
        FUNCTION_ENTRY( "BvsStage" );

        std::ostringstream detailsStr;

        // build the location description

        if ( false == m_locations.empty() )
        {
            try
            {
                detailsStr << LocationCache::getInstance().getLocationByKey( *( m_locations.begin() ) );
            }
            catch (...)
            {
                detailsStr << *( m_locations.begin() );
            }
            try
            {
                detailsStr << " to " << LocationCache::getInstance().getLocationByKey( *( --m_locations.end() ) );
            }
            catch (...)
            {
                detailsStr << " to " << *( --m_locations.end() );
            }

            m_locationDescription = detailsStr.str();
        }

        FUNCTION_EXIT;
    }


    unsigned long BvsStage::getStageId()
    {
        FUNCTION_ENTRY( "getStageId" );
        FUNCTION_EXIT;
        return m_stageId;
    }


    std::set<unsigned long> BvsStage::getAssociatedLocations()
    {
        FUNCTION_ENTRY( "getAssociatedLocations" );

        // no setter - no need to lock
        FUNCTION_EXIT;
        return m_locations;
    }


    std::string BvsStage::getLocationDescription()
    {
        FUNCTION_ENTRY( "getLocationDescription" );
        FUNCTION_EXIT;
        return m_locationDescription;
    }


    // TD16902
    std::string BvsStage::getStageNumber()
    {
        FUNCTION_ENTRY( "getStageNumber" );

        // as discussed, will get the stage number from entity name
        // and entity name must be something like "DBG_BVS_01"
        std::string stageNumber("");
        std::string stageName = this->getName();

        std::string::size_type index;
        std::string::size_type tmpIndex = stageName.find('_', 0);

        if (tmpIndex != std::string::npos)
        {
            index = stageName.find('_', tmpIndex+1);
        }

        if (index != std::string::npos)
        {
            // assume stage number is not more than 99
            stageNumber  = stageName.substr(index+1, 2);
        }

        FUNCTION_EXIT;
        return stageNumber;
    }


    // TD16902

    unsigned short BvsStage::getActiveTrain()
    {
        FUNCTION_ENTRY( "getActiveTrain" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_activeTrain;
    }


    unsigned long BvsStage::getNumActiveMonitors()
    {
        FUNCTION_ENTRY( "getNumActiveMonitors" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_numActiveMonitors;
    }


    void BvsStage::setNumActiveMonitors(unsigned long numActive)
    {
        FUNCTION_ENTRY( "setNumActiveMonitors" );

        ThreadGuard guard( m_lock );
        m_numActiveMonitors = numActive;

        FUNCTION_EXIT;
    }


    void BvsStage::setActiveTrain(unsigned short newActiveTrain)
    {
        FUNCTION_ENTRY( "setActiveTrain" );

        ThreadGuard guard( m_lock );
        m_activeTrain = newActiveTrain;
        m_preConflict = false;

        FUNCTION_EXIT;
    }


    bool BvsStage::isInPreConflict()
    {
        FUNCTION_ENTRY( "isInPreConflict" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_preConflict;
    }


    TrainVideoSourceType BvsStage::getActiveVideoSource()
    {
        FUNCTION_ENTRY( "getActiveVideoSource" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_activeVideoSource;
    }


    unsigned long BvsStage::getAssignment1()
    {
        FUNCTION_ENTRY( "getAssignment1" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_assignment1;
    }


    unsigned long BvsStage::getAssignment2()
    {
        FUNCTION_ENTRY( "getAssignment2" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_assignment2;
    }


    unsigned long BvsStage::getAssignment3()
    {
        FUNCTION_ENTRY( "getAssignment3" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_assignment3;
    }


    unsigned long BvsStage::getAssignment4()
    {
        FUNCTION_ENTRY( "getAssignment4" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_assignment4;
    }


    void BvsStage::setPreConflict(bool isInPreConflict)
    {
        FUNCTION_ENTRY( "setPreConflict" );

        ThreadGuard guard( m_lock );
        m_preConflict = isInPreConflict;

        FUNCTION_EXIT;
    }


    void BvsStage::setActiveVideoSource(TrainVideoSourceType newSource, unsigned long id1, unsigned long id2, unsigned long id3, unsigned long id4)
    {
        FUNCTION_ENTRY( "setActiveVideoSource" );

        ThreadGuard guard( m_lock );	
        m_activeVideoSource = newSource;
       
		
			m_assignment1 = id1;
		
			m_assignment2 = id2;
	
			m_assignment3 = id3;
		
			m_assignment4 = id4;
		

        FUNCTION_EXIT;
    }


    std::string BvsStage::getDetails()
    {
        FUNCTION_ENTRY( "getDetails" );

        // reentrant lock - parent class uses the same lock
   /*     ThreadGuard guard( m_lock );

        // get the item details
        std::ostringstream detailsStr;

        // details of current active train - if any
        if (m_activeTrain > 0)
        {
            // get train details
            // get PV/Service string
            // there is no nice way to do this
            CWnd* wnd = AfxGetMainWnd();
            VideoDisplayManagerDlg* mainDialog = dynamic_cast<VideoDisplayManagerDlg*>(wnd);
            VideoSourcesListCtrl::TrainDetails* trainDetails = NULL;

            if (mainDialog != NULL)
            {
                TrainsPage* trainsPage = mainDialog->getTrainsPage();
                if (trainsPage != NULL)
                {
                    trainDetails = trainsPage->getTrainDetails(m_activeTrain);
                }
            }


            if (trainDetails != NULL)
            {
                detailsStr << "Train: " << trainDetails->serviceNumber << "\r\n";
            }
            else
            {
                detailsStr << "Train: " << static_cast<int>(m_activeTrain) << "\r\n";
            }

            // TD16902
			// build stage location description
            detailsStr << "Stage: " << getStageNumber() << "- " << getLocationDescription() << "\r\n";
            // TD16902

            if (m_preConflict)
            {
                detailsStr << "Status: Pre-Conflict" << "\r\n";
            }
            else
            {
                detailsStr << "Status: Active" << "\r\n";
            }

            switch (m_activeVideoSource)
            {
            case Single:
                detailsStr << "Assignment: Camera " << m_assignment1 << "\r\n";
                break;
            case Quad:
                detailsStr << "Assignment: Quad" << "\r\n";
				if(m_assignment1 != 0)
				{
					detailsStr << " 1: Camera " << m_assignment1 << "\r\n";
				}
				else
				{
					detailsStr << " 1: parked\r\n";
				}				
				if(m_assignment2 != 0)
				{
					detailsStr << " 2: Camera " << m_assignment2 << "\r\n";
				}
				else
				{
					detailsStr << " 2: parked\r\n";
				}				
				if(m_assignment3 != 0)
				{
					detailsStr << " 3: Camera " << m_assignment3 << "\r\n";
				}
				else
				{
					detailsStr << " 3: parked\r\n";
				}				
				if(m_assignment4 != 0)
				{
					detailsStr << " 4: Camera " << m_assignment4 << "\r\n";
                }
				else
				{
					detailsStr << " 4: parked\r\n";
				}				
				break;
            case Sequence:
                detailsStr << "Assignment: Sequence " << m_assignment1 << "\r\n";
                break;
            case None:
                // There is no source - or the source is unknown
                break;
            }
        }
        else
        {
            detailsStr << "No Active Train";
        }

        FUNCTION_EXIT;
        return detailsStr.str();*/
		return "";
    }


} // TA_App


