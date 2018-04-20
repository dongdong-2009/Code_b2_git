/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/ITrainListCtrlEventListener.h $
  * @author:   Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $Date: 2009/11/20 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(ITrainListCtrlEventListener_H)
#define ITrainListCtrlEventListener_H


namespace TA_IRS_Bus
{
	// forward declaration
    class TrainListCtrl;

    /**
     * This is implemented by the TrainSelectorGUI to be notified of when a train list is attempting to scroll, or when something changes.
     * Ie, it handles MFC messages it needs to know about.
     * @version 1.0
     * @created 18-Mar-2008 5:46:18 PM
     */
	
    class ITrainListCtrlEventListener
    {

    public:

        ITrainListCtrlEventListener() 
		{
        }


        virtual ~ITrainListCtrlEventListener() 
		{
        }


        /**
          * Called by a list when it receives an OnVScroll.
         * The list should not call CListCtrl::OnVScroll, that will be done via the redirected scroll method.
          *
          * @param list     The list this comes from
          * @param nSBCode  from MFC
          * @param nPos     from MFC
         * @param pScrollBar    from MFC
         */
        virtual void listScrolledV(TrainListCtrl* list, UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) =0;


        /**
         * Called when a mouse wheel event is received by a train list
         * 
         * @param list    The list this comes from
         * @param nFlags    from MFC
         * @param zDelta    from MFC
         * @param pt    from MFC
         */
        virtual BOOL onMouseWheel(TrainListCtrl* list, UINT nFlags, short zDelta, CPoint pt) =0;


        /**
         * Manages scrolling by keypress
         * 
         * @param list    The list this comes from
         * @param nChar    from MFC
         * @param nRepCnt    from MFC
         * @param nFlags    from MFC
         */
        virtual void onKeyDown(TrainListCtrl* list, UINT nChar, UINT nRepCnt, UINT nFlags) =0;


        /**
         * Called when the column of a list is clicked
         * 
         * @param list    The list this comes from
         * @param pNMHDR    from MFC
         * @param pResult    from MFC
         */
        virtual void onColumnclick(TrainListCtrl* list, NMHDR* pNMHDR, LRESULT* pResult) =0;


        /**
         * Called when a list is clicked
         * 
         * @param list    The list this comes from
         * @param pNMHDR    from MFC
         * @param pResult    from MFC
         */
        virtual void onClick(TrainListCtrl* list, NMHDR* pNMHDR, LRESULT* pResult) =0;


        /**
         * Called when a list item is changed
         * 
         * @param list    The list this comes from
         * @param pNMHDR    from MFC
         * @param pResult    from MFC
         */
        virtual void onItemchanged(TrainListCtrl* list, NMHDR* pNMHDR, LRESULT* pResult) =0;


        /**
         * Called when a train list is trying to determine whether a train should be enabled for selection or not.
         * 
         * @param list    The list this comes from
         * @param trainId    The train Id to check
         * @param trainLocation    the location of the train
         * @param rowNumber    The row the train is on in the list
         */
        virtual bool onIsSelectable(TrainListCtrl* list, TA_IRS_Bus::CommonTypes::TrainIdType trainId, unsigned long trainLocation, int rowNumber, unsigned long trainCommunicationLocation ) =0; //limin++, ECP-338 CL-20150

    };

}

#endif // !defined(ITrainListCtrlEventListener_H)

