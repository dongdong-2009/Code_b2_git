#pragma once

#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectionListener.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"

namespace TA_IRS_App
{
    class PIDSTrainSelectorGUI: public TA_IRS_Bus::ITrainSelectorGUI
    {
    public:
        PIDSTrainSelectorGUI( TA_IRS_Bus::TrainSelectorGUI *m_parentSelGui, TA_IRS_Bus::ITrainSelectionListener * trainSelListener );
        void notifySelectionChanged();
        virtual ~PIDSTrainSelectorGUI() {}
        virtual void updateInboundList( EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails );
        virtual void updateOutboundList( EAction action, const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails );
        virtual void updateSelectionCombo( const TA_IRS_Bus::TrainInformationTypes::TrainSelectionList& trainListNames );
        virtual void updateFallbackStatus( bool inFallback );
        virtual bool hasRadioGroupSupport();
        virtual void updateRadioGroupCombo( const std::vector<std::string>& trainListNames );
        virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getSelectedTrainData() const;
    private:
        TA_IRS_Bus::ITrainSelectionListener * m_trainSelListener;
        TA_IRS_Bus::TrainSelectorGUI *m_parentSelGui;
        bool m_trainSel;
    };
}
