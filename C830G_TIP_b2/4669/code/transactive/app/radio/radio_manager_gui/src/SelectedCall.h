/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/SelectedCall.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// SelectedCall.h: interface for the CSelectedCall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTEDCALL_H__ED650A03_656B_11D7_B165_0050BAB1D931__INCLUDED_)
#define AFX_SELECTEDCALL_H__ED650A03_656B_11D7_B165_0050BAB1D931__INCLUDED_

#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainRadioGroupCorbaProxy.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <set>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSelectedCall : public TA_IRS_Bus::ITrainRadioGroupUpdateObserver
{
    protected:

        CSelectedCall();

        enum EDisplayedCallType
        {
            CALL_NONE,
            NORMAL_CALL,
            FREE_CALL,
            DYNAMIC_GROUP
        };

	public:

		bool IsGroup();
        bool isDynamicGroup() const { return DYNAMIC_GROUP == m_displayedCallType; }

        /**
         * @return true provided we have selected the dynamic group column 
         * AND there are actually items in the dynamic group
         */
        bool isNonEmptyDynamicGroupSelected() const;

        /**
        * If there are any trains involved in the call to be set up, this will set up and make the call.
        * If there are no trains involved, this will return false, and normal radio call set up procedure can take place.
        *
        * @return true if this is a train call
        *
        * @param callType    The call type
        * @param callPriority    The call priority
        */
        bool prepareTrainDriverCall(const TA_IRS_Bus::CallTypes::ECallType callType, const TA_IRS_Bus::CallTypes::ECallPriority callPriority);


        /**
          * answerDriverCall
          *
          * Before setting up audio need to send an SDS ack to TIMS.
          *
          * @return
          */
        void answerDriverCall(TA_IRS_Bus::CommonTypes::TrainIdType trainId);


        virtual ~CSelectedCall();

        // Get the singleton
        static CSelectedCall* GetInstance();
        static void removeInstance();

        // Get area details
        bool    IsCallDisplayed(void);
        bool    IsHotLine();
        CString GetTetraID() const;
        CString GetID();
        CString GetLocation();

        // Update the area
        void    Clear(void);
        void    SetDynamicGroupCall(CString& label);
        void    SetCall(CString& tsiNumber, CString& label, bool hotline);
        void    setCallDisplayed(bool callDisplayed);
        void    saveFreeCall(void);
        void    setHotLine(bool isHotLine);

        /**
          *  SetIsGroup
          *
          * Set whether or not the call is a group call
          *
          * @param bool isGroup True if the call is a group cal, false otherwise.
          */
        void SetIsGroup(bool isGroup);

        /**
         *
         * This must be used prior to placing any calls to train, to ensure
         *  we're using the appropriate (active) TSI for that train.  The input
         *  TSI may no represent the inactive cabin, in which case the return value
         *  is the active cabin that any calls should be placed to
         *
         * @return - if the input TSI is not a train TSI, the input argument is returned
         *         - otherwise returns the 'active' train TSI
         *
         * @param isTrainTetraId[OUT, Optional] set to true if the input TSI is recognised
         *              as a train TSI (this does not necessarily mean the
         *              returned TSI is different from the input TSI), false otherwise
         *
         * ExceptionChecked
         */
        CString getCorrectedTetraId(const CString& tetraId, bool* isTrainTetraId = 0) const;

        /**
         * @return the corrected TSI for the TSI designated by method GetTetraID()
         */
        CString getCorrectedTetraId() const;


        /**
         * This will receive a TrainRadioGroup message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainRadioGroup( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& event );

    
    private:


        /**
        * Given a train TSI (pimary or secondary) get the train details for it.
        *
        * @return the details of the train
        *
        * @exception InvalidTrainException if the TSI does not belong to a train
        *
        * @param tsi    the tetra ID of the train we're trying to identify
        */
        TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetailsFromTSI(const CString& tsi);


        TA_IRS_Bus::CommonTypes::TrainIdType getTrainIdFromTSI(const CString& tsi);


        /**
         * This will ensure the train agent proxy is created and ready to use
         */
        void initialiseTrainAgentProxy();


        /**
        * This takes the current contents of the dynamic group, and separates its members between normal subscribers and trains.
        *
        * @param radioSubscribers    This will contain the individual subscriber ITSIs
        * @param trainSubscribers    This will contain a list of trains within the dynamic group
        */
        void separateTrainsFromDynamicGroupMembers(TA_IRS_Bus::CallTypes::ItsiList& radioSubscribers, TA_IRS_Bus::CommonTypes::TrainIdList& trainSubscribers);


        /**
        * This gets the members of the selected static group, and returns the members that are trains.
        *
        * @param trainSubscribers    This will contain a list of trains within the dynamic group
        */
        void getTrainsFromStaticGroupMembers(TA_IRS_Bus::CommonTypes::TrainIdList& trainSubscribers);


        // class instance
        static CSelectedCall* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;


        TA_IRS_Bus::TrainDriverCallCorbaProxyPtr m_trainDriverCallProxy;
        TA_IRS_Bus::TrainRadioGroupCorbaProxyPtr m_trainRadioGroupCorbaProxy;

        CString GetControlText(int nID) const;

        bool            m_callDisplayed;
        //bool            m_freeCallDisplayed;
        EDisplayedCallType   m_displayedCallType;
        bool            m_groupCall;
        bool            m_isHotLine;

        TA_Base_Core::ReEntrantThreadLockable m_radioGroupsLock;
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap m_radioGroups;

};

#endif // !defined(AFX_SELECTEDCALL_H__ED650A03_656B_11D7_B165_0050BAB1D931__INCLUDED_)
