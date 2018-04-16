/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/IGenericBroadcastPage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * IGenericBroadcastPage is interface for use by generic broadcast model
  *
  */

#if !defined(AFX_IGenericBroadcastPage_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define AFX_IGenericBroadcastPage_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

class IGenericBroadcastPage
{
public:

    // Max # characters allowed to enter into filter text boxes
    // (taken from design doc)
    enum { MAX_FILTER_TEXT = 40 };
        
    /**
     * setTimeStartDtp
     *
     * Updates the start time controls to the new date
     *
     * @param time the new date to apply to the start time
     *
     */
    virtual void setTimeStartDtp(const CTime& time) = 0;

    /**
     * setTimeStopDtp
     *
     * Updates the stop time controls to the new date
     *
     * @param time the new date to apply to the start time
     *
     */
    virtual void setTimeStopDtp(const CTime& time) = 0;


	virtual void updateExecuteButtonEnabledState() = 0;

    virtual CWnd*       getWindow() = 0;

protected:

    /**
     * processStartStopTimeChange
     *
     * Must implement (and call) a function that validates
     * the values entered into the start and stop times of broadcast
     * dialog (relevant for DVA section only)
     */
    virtual void processStartStopTimeChange() = 0;

};

#endif // !defined(AFX_IGenericBroadcastPage_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
