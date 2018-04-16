#ifndef ACTIVESTEP_H_INCLUDED
#define ACTIVESTEP_H_INCLUDED

/**
*  The source code in this file is the property of
*  Ripple Systems and is not for redistribution
*  in any form.
*
*  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h $
*  @author:  Gregg Kirkpatrick
*  @version: $Revision: #10 $
*
*  Last modification: $DateTime: 2014/11/04 13:46:22 $
*  Last modified by:  $Author: qi.huang $
*
*  <description>
*/

#include <string>
#include <list>
#include <vector>
#include <map>

namespace TA_Base_Core
{
    struct StepCustomisableDetail;
}

namespace TA_Base_App
{

class IActivePlan;
class IActiveStepMessage;
class BaseStepParameter;

//Class declaration
class ActiveStep
{
    // Public methods
public:
    ActiveStep( BaseStepParameter& refParameter, IActivePlan& flowControl );
    virtual ~ActiveStep();

    /**
    * execute
    *
    * Allow a client to execute any step in a uniform manner
    * Return state change flag, true if step external state changed
    */
    void execute( bool& bStateChanged, bool& bRemarkChanged );
    void init();

    bool pause();
    bool resume();

    /**
    * id
    *
    * each step must know it's own number
    *
    */
    unsigned int id() const;
    unsigned int pos() const;
    unsigned int pkey() const;
    std::string getRemark();
    std::string getName();
    int getState();
    void stateUpdated( const int nNewState );

    // Overwrite and return custom error event if needed
    virtual IActiveStepMessage* getErrorEvent();

    /**
    * customise
    *
    * change a step's customizable behavior
    * (e.g. skip, delay)
    *
    */
    virtual bool customise( const TA_Base_Core::StepCustomisableDetail& newDetail );

    /**
    * refreshStartTime
    *
    * update the step whenever step is resume or start
    *
    */
    void refreshStartTime();

    /**
    * remark
    *
    * update the remark
    *
    */
    void remark( const std::string& strRemark, const bool bFinal = true );

    virtual std::list<ActiveStep*> getNextStep();
    virtual bool hasNextStep();

    bool completed();
    bool isFailed();
    bool isPaused();
    bool isIgnoreFailure();
    bool isParallel();

    void pushOutgoingStep( ActiveStep& refStep );
    void pushIncomingStep( ActiveStep& refStep );

    void populateParameter( std::vector<std::string>& vecParams );

    enum StepWaitState
    {
        eWaitUndefined = 0,
        eAsyncWaitContinue,
        eWaitSucceed,
        eWaitFailed
    };

protected:
    ActiveStep& operator=( const ActiveStep& );
    ActiveStep( const ActiveStep& );

    /**
    * initImpl
    * where the work is initialized.
    */
    virtual void initImpl();

    /**
    * executeImpl
    * where the work is done.
    */
    virtual bool executeImpl() = 0;

    /**
    * waitImpl
    * waiting for the reply of the execution, return true if reply received, otherwise return false
    */
    virtual int waitImpl();

    virtual bool checkImpl();

    std::string dumpContext() const;

    bool initialize();
    bool delaying();
    bool validate();
    bool executing();
    bool waiting();
    bool paused();
    bool failed();
    bool succeed();

    enum StepState
    {
        eInitialState = 0,
        eLoadPaused,
        eDelaying,
        eDelayPaused,
        eValidating,
        eValidatePaused,
        eExecuting,
        eWaiting,
        eFailed,
        eFailedPaused,
        eFailedCompleted,
        eSucceed,
        eSucceedCompleted,
        eSkipped
    };

    int toExternalState( const StepState eState );
    StepState fromExternalState( const int nExtState );

protected:
    std::list<ActiveStep*> m_lstOutgoingStep;
    std::list<ActiveStep*> m_lstIncomingStep;

    std::string m_remark;

    IActivePlan& m_iPlan;

    time_t m_tmStartTime;

    // Time that the delay started.
    time_t m_delayStartTime;
    // Amount of the delay completed. (Need to keep track of this in case the step is paused in the middle of a delay.)
    time_t m_delayCompleted;

    long m_lWaitingInterval;

    bool m_bRemarkChanged;
    bool m_bCustomizedRemark;
    bool m_bCompleted;

    bool m_bPendingPause;
    bool m_bPendingResume;

    bool m_bParallel;

    StepState m_eState;

    BaseStepParameter& m_refParameter;
}; // ActiveStep

}; // TA_Base_App

#endif // ACTIVESTEP_H_INCLUDED
