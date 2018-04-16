/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/PidSelectionManager.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This handles the selection/deselection of PID buttons
  * on the display page.
  *
  */

#include "StdAfx.h"
#include "PidSelectionManager.h"
#include "IPidSelectionListener.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "bus/mfc_extensions/src/toggleButton/toggleButton.h"

namespace TA_IRS_App
{

    using TA_Base_Core::DebugUtil;


    PidSelectionManager::PidSelectionManager(IPidSelectionListener& selectionListener,
                        CButton& selectAllPids,
                        CButton& clearSelection,
                        const std::vector<ToggleButton*>& allCarButtons,  // TES799 ~ modified
                        const std::vector<ToggleButton*>& pidButtons)
      : m_selectionListener(&selectionListener),
        m_selectAllPids(&selectAllPids),
        m_clearSelection(&clearSelection),
        m_allCarButtons(allCarButtons),
        m_pidButtons(pidButtons)
    {
        FUNCTION_ENTRY( "PidSelectionManager" );

        TA_ASSERT(m_allCarButtons.size() == 3,
                  "Wrong number of all car buttons");

        TA_ASSERT(m_pidButtons.size() == 12,
                  "Wrong number of PID buttons");

        this->clearSelection();

        FUNCTION_EXIT;
    }


    PidSelectionManager::~PidSelectionManager()
    {
        FUNCTION_ENTRY( "~PidSelectionManager" );
        FUNCTION_EXIT;
    }


    void PidSelectionManager::pidButtonPressed()
    {
        FUNCTION_ENTRY( "pidButtonPressed" );

        // a button has been pressed, either up or down

        bool oneIsDown = false;

        for (std::vector<ToggleButton*>::iterator iter = m_pidButtons.begin();
             iter != m_pidButtons.end(); iter++)
        {
            // if the button exists, and its check state is 1 (checked)
            if ( ( (*iter) != NULL ) &&
                 ( (*iter)->isChecked() ) )
            {
                // set the flag
                oneIsDown = true;

                // dont need to check the rest
                break;
            }
        }

        // notify the observer
        m_selectionListener->pidSelectionChanged(oneIsDown);

        FUNCTION_EXIT;
    }


    void PidSelectionManager::selectAllCar(int carNumber)
    {
        FUNCTION_ENTRY( "selectAllCar" );

        TA_ASSERT( (carNumber > 0) && (carNumber < 4),
                    "Invalid car number");

        // select all PIDs for this car
        int startIndex = (carNumber - 1) * 4;
        int endIndex = (carNumber * 4);

        for (int i = startIndex; i < endIndex; i++)
        {
            m_pidButtons[i]->setCheck(true);
        }

        m_allCarButtons[carNumber - 1]->setCheck(true); // TES799 ~ added

        // notify the observer
        m_selectionListener->pidSelectionChanged(true);

        FUNCTION_EXIT;
    }


    // TES799 ~ added method
    void PidSelectionManager::deselectAllCar(unsigned int carNumber)
    {
        FUNCTION_ENTRY( "deselectAllCar" );

        TA_ASSERT((carNumber > 0) && (carNumber <= m_allCarButtons.size()),
                  "Invalid car number");

        // deselect all PIDs for this car
        unsigned int startIndex = (carNumber - 1) * 4;
        unsigned int endIndex = (carNumber * 4);

        for (unsigned int i = startIndex; i < endIndex; ++i)
        {
            m_pidButtons[i]->setCheck(false);
        }

        m_allCarButtons[carNumber - 1]->setCheck(false);

        pidButtonPressed();

        FUNCTION_EXIT;
    }


    void PidSelectionManager::selectAllPids()
    {
        FUNCTION_ENTRY( "selectAllPids" );

        // select the individual PID buttons
		std::vector<ToggleButton*>::iterator iter = m_pidButtons.begin();
        for ( ; iter != m_pidButtons.end(); iter++)
        {
            // if the button exists, and its check state is 1 (checked)
            if ( (*iter) != NULL )
            {
                (*iter)->setCheck(true);
            }
        }

        // select all the 'all car' buttons
        for (iter = m_allCarButtons.begin(); iter != m_allCarButtons.end(); ++iter)
        {
            // if the button exists then check it's state
            if ( (*iter) != NULL )
            {
                (*iter)->setCheck(true);
            }
        }

        // notify the observer
        m_selectionListener->pidSelectionChanged(true);

        FUNCTION_EXIT;
    }


    void PidSelectionManager::clearSelection()
    {
        FUNCTION_ENTRY( "clearSelection" );

		std::vector<ToggleButton*>::iterator iter = m_pidButtons.begin();
        for (;iter != m_pidButtons.end(); iter++)
        {
            // if the button exists, and its check state is 1 (checked)
            if ( (*iter) != NULL )
            {
                (*iter)->setCheck(false);
            }
        }

        // deselect all the 'all car' buttons
        for (iter = m_allCarButtons.begin(); iter != m_allCarButtons.end(); ++iter)
        {
            // if the button exists then uncheck it's state
            if ((*iter) != NULL)
            {
                (*iter)->setCheck(false);
            }
        }

        // notify the observer
        m_selectionListener->pidSelectionChanged(false);

        FUNCTION_EXIT;
    }


    /**
      * getSelectedPIDs
      *
      * Gets a list of selected PIDs.
      * This is done in such a way that calls to the agent will be minimised.
      * eg if selected, entire train, or entire cars, over individual PIDs.
      *
      * @return a list of selected PIDs. The most efficient way possible.
      */
    std::vector<TA_Base_Core::EPIDSelection> PidSelectionManager::getSelectedPIDs() const
    {
        FUNCTION_ENTRY( "getSelectedPIDs" );

        std::vector<TA_Base_Core::EPIDSelection> selectedPIDs;


        // check all car 1
        bool allCar1 = (m_pidButtons[0]->isChecked()) &&
                       (m_pidButtons[1]->isChecked()) &&
                       (m_pidButtons[2]->isChecked()) &&
                       (m_pidButtons[3]->isChecked());


        // all car 2
        bool allCar2 = (m_pidButtons[4]->isChecked()) &&
                       (m_pidButtons[5]->isChecked()) &&
                       (m_pidButtons[6]->isChecked()) &&
                       (m_pidButtons[7]->isChecked());

        // all car 3
        bool allCar3 = (m_pidButtons[8]->isChecked()) &&
                       (m_pidButtons[9]->isChecked()) &&
                       (m_pidButtons[10]->isChecked()) &&
                       (m_pidButtons[11]->isChecked());


        // if the entire train is selected
        if ( allCar1 && allCar2 && allCar3 )
        {
            // this is the easiest case
            selectedPIDs.push_back(TA_Base_Core::ENTIRE_TRAIN);
        }
        else
        {
            // otherwise, check entire cars

            // check car 1
            if ( allCar1 )
            {
                // all car 1 is selected
                selectedPIDs.push_back(TA_Base_Core::ALL_CAR1);
            }
            else
            {
                // not all car 1 is selected
                // check individual PIDs in car 1
                if (m_pidButtons[0]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR1_PID1);
                }
                if (m_pidButtons[1]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR1_PID2);
                }
                if (m_pidButtons[2]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR1_PID3);
                }
                if (m_pidButtons[3]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR1_PID4);
                }
            }

            // check car 2
            if ( allCar2 )
            {
                // all car 2 is selected
                selectedPIDs.push_back(TA_Base_Core::ALL_CAR2);
            }
            else
            {
                // not all car 2 is selected
                // check individual PIDs in car 2
                if (m_pidButtons[4]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR2_PID1);
                }
                if (m_pidButtons[5]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR2_PID2);
                }
                if (m_pidButtons[6]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR2_PID3);
                }
                if (m_pidButtons[7]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR2_PID4);
                }
            }

            // check car 3
            if ( allCar3 )
            {
                // all car 3 is selected
                selectedPIDs.push_back(TA_Base_Core::ALL_CAR3);
            }
            else
            {
                // not all car 3 is selected
                // check individual PIDs in car 3
                if (m_pidButtons[8]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR3_PID1);
                }
                if (m_pidButtons[9]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR3_PID2);
                }
                if (m_pidButtons[10]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR3_PID3);
                }
                if (m_pidButtons[11]->isChecked())
                {
                    selectedPIDs.push_back(TA_Base_Core::CAR3_PID4);
                }
            }

        }

        FUNCTION_EXIT;
        return selectedPIDs;
    }


} // TA_IRS_App

