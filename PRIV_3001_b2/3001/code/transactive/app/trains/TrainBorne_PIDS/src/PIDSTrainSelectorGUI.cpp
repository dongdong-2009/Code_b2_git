#include "stdafx.h"
#include "PIDSTrainSelectorGUI.h"

using namespace TA_IRS_Bus;

namespace TA_IRS_App
{
    PIDSTrainSelectorGUI::PIDSTrainSelectorGUI( TrainSelectorGUI *parentSelGui, ITrainSelectionListener * trainSelListener )
        : m_parentSelGui( parentSelGui ), m_trainSelListener( trainSelListener ), m_trainSel( false )
    {

    }

    TA_IRS_Bus::TrainInformationTypes::TrainDetailsList PIDSTrainSelectorGUI::getSelectedTrainData() const
    {
        return TA_IRS_Bus::TrainInformationTypes::TrainDetailsList();
    }

    void PIDSTrainSelectorGUI::updateRadioGroupCombo( const std::vector<std::string>& trainListNames )
    {

    }
    bool PIDSTrainSelectorGUI::hasRadioGroupSupport()
    {
        return true;
    }

    void PIDSTrainSelectorGUI::updateInboundList( EAction action, const TrainInformationTypes::TrainDetails& trainDetails )
    {
        notifySelectionChanged();
    }
    void PIDSTrainSelectorGUI::updateOutboundList( EAction action, const TrainInformationTypes::TrainDetails& trainDetails )
    {
        notifySelectionChanged();
    }
    void PIDSTrainSelectorGUI::updateSelectionCombo( const TrainInformationTypes::TrainSelectionList& trainListNames )
    {
        notifySelectionChanged();
    }
    void PIDSTrainSelectorGUI::updateFallbackStatus( bool inFallback )
    {
        notifySelectionChanged();
    }

    void PIDSTrainSelectorGUI::notifySelectionChanged()
    {
        FUNCTION_ENTRY( "notifySelectionChanged" );

        CommonTypes::TrainIdList selectedTrains = m_parentSelGui->getSelectedTrains();
        bool selectionExists = selectedTrains.size() > 0;

        if ( m_trainSel != selectionExists )
        {
            m_trainSel = selectionExists;
            if ( m_trainSelListener != NULL )
            {
                // tell it whether trains are selected
                m_trainSelListener->trainSelectionChanged( m_trainSel );
            }
        }

        FUNCTION_EXIT;
    }

}
