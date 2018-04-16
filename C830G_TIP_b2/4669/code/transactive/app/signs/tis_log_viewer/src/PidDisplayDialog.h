/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/PidDisplayDialog.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Used to display destination PIDs.
  *
  */

#if !defined(AFX_PIDDISPLAYDIALOG_H__35AB034E_8C19_4A1F_90C9_FAA6F3A84769__INCLUDED_)
#define AFX_PIDDISPLAYDIALOG_H__35AB034E_8C19_4A1F_90C9_FAA6F3A84769__INCLUDED_

#include <string>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PidDisplayDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PidDisplayDialog dialog

namespace TA_IRS_App
{
    class PidDisplayDialog : public CDialog
    {
    // Construction
    public:
        PidDisplayDialog(CWnd* pParent = NULL);   // standard constructor

        /**
          * displayDialog
          *
          * Display the dialog with the given details.
          *
          * @param destinationTrain     the destination train
          * @param tTisDestinationPids  A space delimited list of PIDs
          *                             (Correspond to EPIDSelections)
          *
          */
        void displayTtisDestinations(const std::string& destinationTrain,
                                     const std::string& tTisDestinationPids);


        /**
          * displayStisDestinations
          *
          * Display the dialog with the given details.
          *
          * @param sTisDestinationStation   The detination station
          * @param sTisDestinationLevels    The destinatin level(s)
          *                                 (as a space delimited string)
          * @param sTisDestinationPids      The destination PIDs
          *                                 (as a space delimited string)
          *
          */
        void displayStisDestinations(const std::string& sTisDestinationStation,
                                     const std::string& sTisDestinationLevels,
                                     const std::string& sTisDestinationPids);

    // Dialog Data
        // {{AFX_DATA(PidDisplayDialog)
    enum { IDD = IDD_DESTINATION_DIALOG };
    CListBox    m_pidList;
    CString    m_destination;
    // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(PidDisplayDialog)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(PidDisplayDialog)
    afx_msg void OnClose();
    virtual BOOL OnInitDialog();
    afx_msg void OnSetfocusDestination();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        // TES885 ~ added
        /**
          * getAllStationPIDs
          *
          * Returns a list of all the PIDs at the specified station.
          *
          * @return std::vector<std::string>
          * @param : const std::string& theStation
          */
        std::vector<std::string> getAllStationPIDs(const std::string& theStation);

        // TES885 ~ added
        /**
          * tokenizePIDName
          *
          * Splits a pid or level name on the '.' characters.
          *
          * @return std::vector<std::string>
          *     a vector of the strings between the '.' characters
          * @param : std::string pidEntityName
          *     the full asset name eg DBG.TIS.Platform.PDP01 or DBG.TIS.Platform
          */
        std::vector<std::string> tokenizePIDName(std::string pidEntityName);

    private:

        std::vector<std::string> m_pidVector;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_App

#endif // !defined(AFX_PIDDISPLAYDIALOG_H__35AB034E_8C19_4A1F_90C9_FAA6F3A84769__INCLUDED_)
