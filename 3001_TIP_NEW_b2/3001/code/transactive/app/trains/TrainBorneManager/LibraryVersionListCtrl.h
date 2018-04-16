/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/LibraryVersionListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * This list is formatted to display library versions.
  */

#if !defined(LibraryVersionListCtrl_H)
#define LibraryVersionListCtrl_H

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

#include "bus/signs/tis_agent_access/src/TTISLibraryVersionMonitor.h"
#include "bus/signs/tis_agent_access/src/ILibraryVersionListener.h"

namespace TA_Base_Core
{
	class ILocation;
}

namespace TA_IRS_App
{
    /**
     * This ListCtrl is responsible for displaying the TTIS versions at
     * stations.
     *
     * It inherits from TA_IRS_Bus::ILibraryVersionListener because it needs
     * to know when a library version has changed.  This is done through the
     * implementation of libraryVersionUpdate method.  When this method is
     * invoked, this ListCtrl will update its internal version tables and
     * send itself a WM_USER message.  This will cause the event pump to
     * invoke the OnLibraryVersionUpdate method, which will change the GUI
     * elements to match.  Finally, this will post a WM_PAINT message so that
     * the ListCtrl can redraw itself on the screen.
     *
     * This ListCtrl is not editable, since all data comes from the system.
     */
    class LibraryVersionListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus,
                                   public TA_IRS_Bus::ILibraryVersionListener
    {
    public:

	    LibraryVersionListCtrl();
	    virtual ~LibraryVersionListCtrl();

		void Init();

    public:

        /**
         * Observer method that will be invoked a library version has been
         * updated.
         * @param libraryVersion The library version data.
         */
        virtual void libraryVersionUpdate( TA_IRS_Bus::LibraryVersionUpdateInfo libraryVersion );

		afx_msg LRESULT OnLibraryVersionUpdate(WPARAM updateInfo, LPARAM nouse);
	    DECLARE_MESSAGE_MAP()

    protected:


        // the columns for the list control
        enum VersionListColumns
        {
            Station = 0,
            CurrentISCS = 1,
            NextISCS = 2
        };

        virtual bool isSelectable(int rowNumber);

    private:

        // the custom header control
        TA_Base_Bus::FixedHeaderCtrl	m_fixedHeaderCtrl;

		std::string m_locationName;

	private:

		int initLocationItems(TA_Base_Core::ILocation* location, int insertIndex);
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_App

#endif // !defined(LibraryVersionListCtrl_H)
