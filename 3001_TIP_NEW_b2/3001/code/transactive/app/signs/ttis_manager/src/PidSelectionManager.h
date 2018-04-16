/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/PidSelectionManager.h $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This handles the selection/deselection of PID buttons
  * on the display page.
  *
  */


#if !defined(PIDSELECTIONMANAGER_H)
#define PIDSELECTIONMANAGER_H


#include <vector>
#include "core/data_access_interface/tis_agent_4669/IDL/src/TisMessageDataDefinitions.h"

#include "bus/mfc_extensions/src/toggleButton/toggleButton.h"

namespace TA_IRS_App
{
    // forward declaration
    class IPidSelectionListener;

    class PidSelectionManager
    {

    public:

        /**
          * PidSelectionManager
          *
          * Constructor that takes PID buttons.
          *
          * @param selectionListener Who needs to know when PID selection changes
          * @param selectAllPids    The button sed to select all PIDs
          * @param clearSelection   The button used to clear PID selection
          * @param allCarButtons    The buttons used to select entire cars (asserts if there arent 3)
          * @param pidButtons       Each individual PID button (asserts if there arent 12)
          *
          */
        PidSelectionManager(IPidSelectionListener& selectionListener,
                            CButton& selectAllPids,
                            CButton& clearSelection,
                            const std::vector<ToggleButton*>& allCarButtons,  // TES799 ~ modified
                            const std::vector<ToggleButton*>& pidButtons);


        ~PidSelectionManager();


        /**
          * pidButtonPressed
          *
          * Should be called when one of the individual PID buttons
          * is pressed.
          */
        void pidButtonPressed();

        /**
          * selectAllCar
          *
          * Called when a select car button is pressed.
          *
          * @param carNumber    The car number, must be 1, 2 or 3
          */
        void selectAllCar(int carNumber);

        // TES799 ~ added
        /**
          * deselectAllCar
          *
          * Called when a 'select all car' button is depressed.
          *
          * @param carNumber    The car number, must be 1, 2 or 3
          */
        void deselectAllCar(unsigned int carNumber);

        /**
          * selectAllPids
          *
          * called when select all PIDs is pressed
          */
        void selectAllPids();


        /**
          * clearSelection
          *
          * Called when clear selection is pressed
          */
        void clearSelection();


        /**
          * getSelectedPIDs
          *
          * Gets a list of selected PIDs.
          * This is done in such a way that calls to the agent will be minimised.
          * eg if selected, entire train, or entire cars, over individual PIDs.
          *
          * @return a list of selected PIDs. The most efficient way possible.
          */
        std::vector<TA_Base_Core::EPIDSelection> getSelectedPIDs() const;


    private:

        PidSelectionManager(){}

        // observer to update if PID selection changes
        IPidSelectionListener* m_selectionListener;

        // pid buttons
        CButton* m_selectAllPids;
        CButton* m_clearSelection;
        std::vector<ToggleButton*> m_allCarButtons;  // TES799 ~ modified
        std::vector<ToggleButton*> m_pidButtons;

    };

} // TA_IRS_App

#endif // !defined(PIDSELECTIONMANAGER_H)
