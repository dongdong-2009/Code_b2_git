/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/incident_rights/src/IncidentRightsGridControl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// IncidentRightsGridControl.h: interface for the IncidentRightsGridControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INCIDENTRIGHTSGRIDCONTROL_H__D11A0970_286E_4AA3_9389_603271DF0849__INCLUDED_)
#define AFX_INCIDENTRIGHTSGRIDCONTROL_H__D11A0970_286E_4AA3_9389_603271DF0849__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
//#include <string>

#define IMPORT_EXTENSIONS
#include "bus/mfc_extensions/src/mfc_extensions.h"

#include "core/data_access_interface/incident_dai/src/ItaReportAccessFactory.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

 
using namespace TA_IRS_Core;
 
namespace TA_IRS_App
{	
	class IncidentRightsGridControl : public CGridCtrl  
	{
	public:

		enum IncidentType
        {
            STATION_REPORT = 0,   // not incident
			INCIDENT_REPORT = 1,   // is incident            
        };

	
		/**
         * Constructor
         */
		IncidentRightsGridControl();
		
		/**
         * Destructor
         */
		virtual ~IncidentRightsGridControl();
 
		/**
         * setStatusBar
         *
         * This lets the DLL know about the status bar so that it can use it as a progress bar
         * when loading large amounts of data.
         *
         * @param IProgressStatusBar* - The status bar to use for displaying a progress bar.
         */	 
		void setStatusBar(TA_Base_App::IProgressStatusBar* statusBar);

		/**
         * setAccessMatrix
         *
         * This is called when the Grid control needs to be updated with the values from the database
         *
         * @param AccessMatrix& - This is the access matrix whose values will be loaded to the grid control
         */
		void setAccessMatrix(AccessMatrix& matrix);

		/**
         * getAccessMatrix
         *
         * Returns an access matrix with the values set in the grid control
         *
         */
		AccessMatrix getAccessMatrix();
	
		
		/**
         * hasChanged
         *
         * Returns true if an item in the grid control has been modified
         *
         */
		bool hasChanged();

	
	
		/**
         * resetDataChanged
         *
         * Resets the flag that determines if a data in the grid control has been modified
         *
         */
		void resetDataChanged()
		{
			m_bDataChanged = false;
		}


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RightsGridControl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(RightsGridControl)
		    // NOTE - the ClassWizard will add and remove member functions here.
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


        /**
         * OnEndEditCell
         *
         * This is an override that is called when the user has finished editing a cell. In here
         * we will update our database objects and store the changes ready for applying later.
         *
         * @param int - The row the user has modified
         * @param int - The column the user has modified
         * @param CString - This is the new value the user has entered for the cell
         */
        virtual void OnEndEditCell(int nRow, int nCol, CString str);


        /**
         * CreateInPlaceEditControl
         *
         * This is an override that is called when the user begins editing a cell. We must override
         * it because we want to create a drop down list rather then just let the user type text in.
         *
         * @param Parameters just need to be passed onto the base implementation of this method
         */
        virtual void CreateInPlaceEditControl(CRect& rect, DWORD dwStyle, UINT nID,
                                                     int nRow, int nCol,
                                                     LPCTSTR szText, int nChar);

private:
	    /**
         * getAccessLevel
         * 
         *  Returns the access level key from the given access type string
         */
		EAccessType getAccessLevelKey(const CString &strAccessType);

		/**
         * setupAccessTypes
         *
         * Builds the m_accessTypes map
         */
		void setupAccessTypes();

        /**
         * setUpHeaders
         * 
         * This sets up the row and column headers after retrieving all the required information
         * from the database
         */
        void setUpHeaders();

        /**
         * getAllProfileGroups
         *
         * Retrieves the profile groups from the database
         */
		std::map<unsigned long, std::string> getAllProfileGroups();

        /**
         * resetGrid
         *
         * Resets all the fields to blank
         */
        void resetGrid();

		// This is the status bar to use for showing progress
        TA_Base_App::IProgressStatusBar* m_statusBar;

 		AccessMatrix m_accessMatrix;	  
		
		std::map<EAccessType, CString> m_accessTypes;

		bool	m_bDataChanged; 

		// need to use this flag to determine if a change in the item data
		// in the grid control is from initialization
		bool	m_bIsReset; 

	};
}

#endif // !defined(AFX_INCIDENTRIGHTSGRIDCONTROL_H__D11A0970_286E_4AA3_9389_603271DF0849__INCLUDED_)
