/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/LibraryVersionListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * This list is formatted to display library versions.
  */

#if !defined(LibraryVersionListCtrl_H)
#define LibraryVersionListCtrl_H

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"

#include "bus/signs/tis_agent_access/src/TTISLibraryVersionMonitor.h"
#include "bus/signs/tis_agent_access/src/ILibraryVersionListener.h"

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
        virtual void libraryVersionUpdate( TA_IRS_Bus::LibraryVersionInfo libraryVersion );

		//hongran++ TD14352

        /**
         * This function is used for setting the current local predefined msg version when setup.
         * .
         * @param version: The library version data.
         */
		void  setLocalCurrentLibVerstion(unsigned long version);

        /**
         * This function is used for setting the next local predefined msg version when setup.
         * .
         * @param version: The library version data.
         */
		void  setLocalNextLibVerstion(unsigned long version);
		//++hongran TD14352

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(LibraryVersionListCtrl)
	    protected:
	    virtual void PreSubclassWindow();
	    //}}AFX_VIRTUAL

	    // Generated message map functions
	    //{{AFX_MSG(LibraryVersionPage)
		afx_msg LRESULT OnLibraryVersionUpdate(WPARAM wParam, LPARAM lParam);
	    //}}AFX_MSG
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

        /**
         * Populates the data.
         */
        void populateData();

        /**
         * Sets up the list.
         */
        void setupList();


        /**
         * Returns the version string that matches the entityKey of the
         * datapoint that is provided.  Will return "N/A" if the entityKey
         * is 0.
         */
        CString getVersionString( unsigned long entityKey );

        // the custom header control
        TA_Base_Bus::FixedHeaderCtrl	m_fixedHeaderCtrl;


        // Map of library version datapoint entity key to current value
        std::map<unsigned long, unsigned short> m_libraryVersionInfo;

        // Entity keys of datapoints at each location
        std::map<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo> m_locationInfo;

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_App

#endif // !defined(LibraryVersionListCtrl_H)
