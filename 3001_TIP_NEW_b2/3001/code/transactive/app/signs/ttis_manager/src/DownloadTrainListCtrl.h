/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/DownloadTrainListCtrl.h $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $Datetime:$
  * Last modified by: $Author: haijun.li $
  * 
  * DownloadTrainListCtrl defines a list control that displays trains
  * and the message library/time schedule version.
  */

#if !defined(AFX_DownloadTrainListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
#define AFX_DownloadTrainListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_

#include "bus/trains/TrainSelectionComponent/src/TrainListCtrl.h"


namespace TA_IRS_App
{
    class DownloadTrainListCtrl : public TA_IRS_Bus::TrainListCtrl
    {
    public:

        DownloadTrainListCtrl();
	    virtual ~DownloadTrainListCtrl();


        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(DownloadTrainListCtrl)
	    //}}AFX_VIRTUAL


	    // Generated message map functions
    protected:

	    //{{AFX_MSG(DownloadTrainListCtrl)
        //}}AFX_MSG
	        
        DECLARE_MESSAGE_MAP()

    public:

        // the names of the custom columns
        static const std::string VERSION_COLUMN;
        static const std::string UPGRADED_COLUMN;

		//TD17545
        static const std::string TRAIN_SETTINGS_ENTITY_TYPE;
        static const std::string TRAIN_SETTINGS_ENTITY_NAME;
		//TD17545

        /** 
          * setupList
          *
          * Set up the custom content of this list.
          *
          * @param versionProvider  The object providing content for the version columns.
          */
        void setupList(TA_IRS_Bus::ICustomTrainDataProvider* versionProvider);


        /**
         * updateTrainVersion
         *
         * Forces the list to update the version for the given train.
         *
         * @param trainId the train id to update
         */
        void updateTrainVersion( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
          * getPercentComplete
          *
          * Gets the % of items in this list that are at the current version
          *
          * @param currentVersion The current version
          *
          * @return the % at the current version
          */
        //xufeng++ TD17545
        unsigned short getPercentComplete(unsigned short currentVersion, int columnid);
        //unsigned short getPercentComplete(unsigned short currentVersion);
        //++xufeng TD17545

        /**
          * getServiceNumberForTrain
          *
          * This list holds train information, for each train.
          * This list shows all trains, so it is more efficient to just
          * get the service number for a train from here, rather than
          * setting up another cache.
          *
          * @param trainId  The train Id to look up the service number for,
          *
          * @return The service number string for the train. "_" if not found.
          */
        std::string getServiceNumberForTrain(TA_IRS_Bus::CommonTypes::TrainIdType trainId);


    protected:

        /**
          * isSelectable
          *
          * Overridden from TA_Bus::ListCtrlOnOffSel
          * Given the row number, return true if this item
          * should be selectable.
          *
          * @param rowNumber the row number.
          *
          * @return true if the item can be selected
          */
        virtual bool isSelectable(int rowNumber);

    private:


        /**
          * isSelectable
          *
          * If the item is selectable, return true.
          * This is used to determine if items should be grayed and
          * disabled.
          *
          * At the OCC - all in service trains are selectable
          * At the depot - all depot trains are selectable
          * At a station - all local trains are selectable in degraded mode.
          *
          * @param itemData the TrainDetails structure for the item
          *
          * @return whether its enabled or disabled.
          */
        bool isSelectable(const TA_IRS_Bus::TrainListCtrl::TrainDetailsItemData& itemData) const;


        int m_versionColumnIndex;
        int m_upgradedColumnIndex;

        //TD17545
        //The total number of trains in the C830. Default value is 40.
        int m_totalTrainNumber;

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_App

#endif // !defined(AFX_DownloadTrainListCtrl_H__0F637CDF_8898_4678_938E_4506A31BBE48__INCLUDED_)
