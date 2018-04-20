/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mfc_extensions/src/combo_temporary_item/ComboTempItem.h $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * This class extends the combo box control to allow a temporary item to be
 * inserted into it. When this item is unselected it is removed from
 * the combo box.
 */

#if !defined(_COMBO_TEMP_ITEM_H_)
#define _COMBO_TEMP_ITEM_H_

#include "../stdafx.h"

namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) ComboTempItem : public CComboBox
    #else
    class AFX_EXT_CLASS ComboTempItem : public CComboBox
    #endif
    {

    public:
        /**
         * Constructor
         */
	    ComboTempItem();

        /**
         * Destructor
         */
	    virtual ~ComboTempItem();

    public:

    // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ComboTempItem)
	//}}AFX_VIRTUAL

        /**
         * AddTempString
         *
         * Adds a temporary item to the combo box. The selection
         * is automatically changed to the new item.
         *
         * @param text The text for the temporary string.
         *
         * @return int The index of the temporary string.
         */
        int AddTempString( CString text );


	// Generated message map functions
    protected:

	//{{AFX_MSG(ComboTempItem)
    afx_msg void OnSelChange();
	//}}AFX_MSG
	    
    DECLARE_MESSAGE_MAP()

    private:
        /**
         * removeTempString
         *
         * Removes the temporary string from the list control.
         */
        void removeTempString();

        // This variable stores the value of the current temporary item.
        CString m_tempString;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_Base_Bus

#endif // !defined(_COMBO_TEMP_ITEM_H_)
