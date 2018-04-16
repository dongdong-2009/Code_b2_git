#ifndef PLAN_STEP_INCLUDE_H
#define PLAN_STEP_INCLUDE_H

#include <vector>
#include <map>

#include "ace/RW_Mutex.h"

class IActivityParameter;

namespace TA_Base_App
{
    class CPlanStepEditParameterPage;
    class BaseStepParameter;
	class IPlanNode;

    class PlanStep
    {
    public:
        PlanStep(IPlanNode* parent, BaseStepParameter& refParameter, bool isNewStep);
        virtual ~PlanStep();

        virtual PlanStep* clone() = 0;
        virtual bool hasCustomIncomingStep() const;
        virtual bool hasCustomOutgoingStep() const = 0;
        virtual bool canIgnoreFailure() const = 0;  // step failure cannot be ignored where its outcome determines
        virtual void createPlanStepParameters() = 0;
        virtual bool isParametersValid( const std::vector<std::string>& parameters ) = 0;
        virtual void resetFlowParameters();
        virtual void clearFlowParameters();
        virtual void updateFlowParameters();
		virtual CPlanStepEditParameterPage* getParameterEditPage();
        virtual std::string getStepTypeName() const = 0;
        virtual bool editable();
        virtual bool hasIncoming();
        virtual bool hasOutgoing();
        virtual bool allowMultipleIncoming();
        virtual bool allowMultipleOutgoing();
        void copyParameter( const std::vector<std::string>& vecParams );
        void populateParameter( const std::vector<std::string>& vecParams );
        std::vector<std::string> compactParameter();
        IPlanNode* getParentPlan() const;
        void reloadParameters();
        virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
        virtual void incomingStepChanged( const unsigned int unPrevIncoming, const unsigned int unNewIncoming );
        virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
        virtual bool retrieveIncomings( std::vector<unsigned int>& vecIncomings );
		virtual bool canParallelHandle();
        void linkOutgoingStep( const unsigned int unPrevOutgoing, const unsigned int unOutgoing );
        void linkIncomingStep( const unsigned int unPrevIncoming, const unsigned int unIncoming );

        // Accessor methods
        unsigned long getStepPkey() const;
        unsigned long getStepId() const;
        unsigned long getStepPosition() const;
		const std::string getStepName() const;
		const std::string getStepDescription() const;
        const std::string getStepGraph() const;
		int getStepType() const;
		bool isStepSkipped() const;
		bool isStepSkippable() const;
		bool ignoreFailure() const;
        time_t getStepDelay() const;

		// Editor methods
		void setStepPosition(const unsigned long stepPosition);
		void setStepName( const std::string& name );
		void setStepDescription( const std::string& description );
		void setStepGraph( const std::string& graph );
		void setStepSkippable(bool skip);
		void setStepSkipped(bool skip);
		void setIgnoreFailure(bool ignoreFailure);
        void setStepDelay( const unsigned long delay );
        void setStepPkey( const unsigned long pkey );
        void setStepId( const unsigned long stepId );

        bool updated();

		virtual IActivityParameter& getActivityParameter() = 0;

    protected:
        bool m_bParametersInited;
        ACE_RW_Mutex m_mtxStepParameters;

        BaseStepParameter& m_refParameter;
        IPlanNode* m_parent;

		// Attributes
	private:
		PlanStep &operator=(const PlanStep &other);

		bool m_newStep;
    };
}

#endif // PLAN_STEP_INCLUDE_H