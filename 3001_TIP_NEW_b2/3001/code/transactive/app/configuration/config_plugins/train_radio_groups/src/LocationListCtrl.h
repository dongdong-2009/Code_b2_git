/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/LocationListCtrl.h $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This class manages the list display used to show the available locations for the train radio group
  * in the TrainRadioGroupDetailView class
  */

#if !defined(LocationListCtrl_H)
#define LocationListCtrl_H


#include "app/configuration/config_plugins/train_radio_groups/src/stdafx.h"

#include "bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h"
#include "bus/mfc_extensions/src/editable_list_control/ICellManager.h"


// forward declarations
namespace TA_IRS_Core
{
    class ITrainRadioGroup;
}


namespace TA_IRS_App
{
    /**
     * @author oliverk
     * @version 1.0
     * @created 15-Jan-2008 12:51:23 PM
     */

    class LocationListCtrl : public ICellManager,
                             public CEditableListCtrl
    {

    public:

        /**
         * Constructor
         */
        LocationListCtrl();


        /**
         * Destructor
         */
        virtual ~LocationListCtrl();


        /**
         * addNewItems
         *
         * This is called when the user has selected the option to add new location to the list.
         * This should show a dialog to the user to allow them to add new location and then add them
         * to the list and the train Radio object
         */
        void addNewItems();


        /**
         * removeSelectedItem
         *
         * This is called when the user has selected the option to remove a location from the list.
         * This should remove the item from the list and from the train Radio object.
         */
        void removeSelectedItem();


        /**
         * GetCellType
         *
         * This determines the type of cell the list control should display. This will be either a
         * normal field (for the managed process name) or a combo box (for the running type).
         *
         * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
         *                             location name or key
         * @param int - This is the currently selected item.
         * @param int - This is the currently selected column. We are only interested if it is the
         *              running type column. Otherwise the user cannot edit it.
         *
         * @return CellType - This returns the combo type
         */
        virtual ICellManager::CellData GetCellData( CEditableListCtrl* listCtrl, int item, int subItem );


        /**
         * GetCellMask
         *
         * This will determine and return the mask to be used for the specified list control cell.
         *
         * @param CEditableListCtrl* - This is the list control.
         * @param int - This is the currently selected item.
         * @param int - This is the currently selected column.
         *
         * @return CString - The mask for the selected item.
         */
        virtual CString GetCellMask( CEditableListCtrl* listCtrl, int item, int subItem );


        /**
         * GetCellMaskPromptChar
         *
         * This will determine and return the mask prompt character to be used for the specified list control cell.
         *
         * @param CEditableListCtrl* - This is the list control.
         * @param int - This is the currently selected item.
         * @param int - This is the currently selected column.
         *
         * @return char - The prompt character.
         */
        virtual char GetCellMaskPromptChar( CEditableListCtrl* listCtrl, int item, int subItem );


        /**
         * GetCellMaximumCharacters
         *
         * Return the type of the specified table cell. For unlimited number of characters this method
         * should return 0. This method is only relevant to CT_TEXT and CT_NUMBER field types.
         *
         * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
         *                             parameter name.
         * @param int - This is the currently selected item.
         * @param int - This is the currently selected column. We are only interested if it is the
         *              value column. Otherwise the user cannot edit it.
         */
        virtual unsigned long GetCellMaximumCharacters( CEditableListCtrl* listCtrl, int item, int subItem );


        /**
         * GetCellType
         *
         * This determines the type of cell the list control should display. This will be either a
         * normal field (for the managed process name) or a combo box (for the running type).
         *
         * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
         *                             process name or key
         * @param int - This is the currently selected item.
         * @param int - This is the currently selected column. We are only interested if it is the
         *              running type column. Otherwise the user cannot edit it.
         *
         * @return CellType - This returns the combo type
         */
        virtual ICellManager::CellType GetCellType( CEditableListCtrl* listCtrl, int item, int subItem );


        /**
         * populateControl
         *
         * This will populate the list control with the current of the Train Radio Group passed
         * in.
         *
         * @param IConfigEntity& - The current TrainRadio Group the user has selected
         */
        void populateControl( TA_IRS_Core::ITrainRadioGroup& currentTrainRadioGroup );


        /**
         * UpdateCellValue
         *
         * Once the user has changed a value then this is called. This must set the value in the
         * IConfigEntity object.
         *
         * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
         *                             process name and key
         * @param int - This is the currently selected item.
         * @param int - This is the currently selected column. We are only interested if it is the
         *              value column. Otherwise the user cannot edit it.
         * @param CString - The new value for the item
         *
         * @return bool - This indicates whether the update worked or not. True indicates everything went
         *                ok and the control can update. False indicates an error occurred and the update
         *                cannot take place.
         */
        virtual bool UpdateCellValue( CEditableListCtrl* listCtrl, int item, int subItem, CString value );


    protected:


        virtual void PreSubclassWindow();


        void setupList();


    private:

        /**
         * These are the set up details for the attribute list control
         */
        static const int LOCATION_NAME_COLUMN;
        static const CString LOCATION_NAME_COLUMN_NAME;
        static const int LOCATION_NAME_COLUMN_WIDTH;


        /**
         * This is the currentTrainRadioGroup we are displaying location for
         */
        TA_IRS_Core::ITrainRadioGroup* m_currentTrainRadioGroup;

    };

}

#endif // !defined(LocationListCtrl_H)
