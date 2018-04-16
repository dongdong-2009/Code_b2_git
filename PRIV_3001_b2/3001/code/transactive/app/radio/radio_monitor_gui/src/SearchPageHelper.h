/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/SearchPageHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Provides necessary interface functions to allow use of CSearchPage
  *
  */

#pragma once

#include <vector>
#include "bus/radio/radiosearchpage/src/isearchpagehelper.h"

namespace TA_IRS_App
{
    class SearchPageHelper : public TA_IRS_Bus::ISearchPageHelper
    {
    public:
        static SearchPageHelper& getInstance();
    
        SearchPageHelper();

		virtual IRadio& getRadioCorbaDef();

        /**
         * getRadioDirectoryCorbaDef (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual IRadioDirectory& getRadioDirectoryCorbaDef();
        
        /**
         * getTrainSelectorGUI (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual TA_IRS_Bus::MFCTrainSelectorGUI& getTrainSelectorGUI();
        virtual TA_IRS_Bus::TrainSelector*  getTrainSelector();

        /**
         * isFunctionPermitted (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual bool isFunctionPermitted(DWORD globalRightIdentifier);
        
        /**
         * SetCall (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual void SetCall(CString& tsiNumber, CString& label, bool hotline, const ESelectionType& type);
        
        /**
         * addDynamicGroupMembers  (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual void addDynamicGroupMembers(std::vector<std::vector<CString> >* selectedSubscribers);    

		/**
         * addMonitors  (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual void addMonitors(std::vector<std::vector<CString> >* selectedSubscribers);   

        
        /**
         * getAddButtonRightIdentifier  (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        DWORD getAddButtonRightIdentifier();

        /**
         * getEntityLocationKey (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual DWORD getEntityLocationKey();

        /**
         * shouldDisplayDynamicGroupNode (interface implementation, @see ISearchPageHelper for more details)
         *
         */
        virtual bool shouldDisplayDynamicGroupNode();

        /**
         * selectedTrainDetailsUpdated
         *
         * Tells the GUI to update this item if it is in the dynamic group list
         * or if it is in the selected TSI list
         */
        virtual void selectedTrainDetailsUpdated( CString& oldTsiNumber,
                                                  CString& oldServiceNumber,
                                                  CString& oldLocation,
                                                  CString& newTsiNumber,
                                                  CString& newServiceNumber,
                                                  CString& newLocation );

    private:

        /// Creates the train selector component, if it hasn't been created already
        void initialiseTrainSelector();

        TA_IRS_Bus::MFCTrainSelectorGUI m_trainSelectorGUI;
        TA_IRS_Bus::TrainSelector*  m_trainSelector;
    };

}
