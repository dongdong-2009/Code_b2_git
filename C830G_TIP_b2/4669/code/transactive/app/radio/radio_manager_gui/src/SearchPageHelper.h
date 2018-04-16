/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/SearchPageHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides necessary interface functions to allow use of CSearchPage
  *
  */

#pragma once

#include <vector>
#include "bus/radio/radiosearchpage/src/isearchpagehelper.h"

namespace TA_IRS_Bus
{
    class CSearchPage;
};

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
        virtual DWORD getAddButtonRightIdentifier();    

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

        /**
         * Helper function to forward queries back to the Subscriber Search Directory
         *  The only notification it needs to be aware of is the removal of dynamic group members
         */
        void removeDynamicGroupSubscriber(CString tsi);
        void addDynamicGroupSubscriber(CString subscriberName, CString tsi);

        /**
         * Used to provide a global link between the actual search page object and the callback
         *  (creates cyclical dependency -- used for convenience)
         */
        void setSearchPage(TA_IRS_Bus::CSearchPage& searchPageWindow);
        TA_IRS_Bus::CSearchPage* getSearchPage() { return m_searchPageWindow; } 

    private:
        
        // A bi-directional link is required between the search page component and the GUI
        // that uses it (at this stage, only to process "Remove From Dynamic Group" requests)
        TA_IRS_Bus::CSearchPage*   m_searchPageWindow;
    };

}
