/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/trainradiogroupcomponent.h $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */
#if !defined(TrainRadioGroupComponent_H)
#define TrainRadioGroupComponent_H


#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"


namespace TA_IRS_App
{
    /**
     * @author oliverk
     * @version 1.0
     * @created 15-Jan-2008 12:51:24 PM
     */

    class TrainRadioGroupComponent : public TA_Base_App::AbstractComponent
    {

    public:


        /**
         * This is the name of the DLL and is used to load resources correctly.
         */
        static const CString DLL_NAME;


        /**
         * Creates the train radio group component
         *
         * @param componentName The name of the component (from the configuration)
         */
        TrainRadioGroupComponent( const std::string& componentName );


        /**
         * Destructor
         */
        virtual ~TrainRadioGroupComponent();


        /**
         * Gets the component view (the right pane of the configuration editor).
         * This is basicall the plugin GUI.
         *
         * @return the component view
         */
        virtual CRuntimeClass* getComponentView();


        /**
         * Gets the icon that is used for this plugin.
         * It goes in the left pane of the config editor
         *
         * @return the icon
         */
        virtual HICON getIcon();


        /**
         * This gets the action required to add this component.
         *
         * @return the right the operator needs to use this component
         */
        virtual unsigned long getRightsActionForAddingThisComponent();


    protected:

        
        /**
         * Gets the name of the DLL for this component
         *
         * @return the dll name
         */
        virtual std::string getDLLName() const;


    private:


        /**
         * Private constructors
         */
        TrainRadioGroupComponent();
        TrainRadioGroupComponent( const TrainRadioGroupComponent& prm1 );
        TrainRadioGroupComponent& operator=( const TrainRadioGroupComponent& prm1 );


        /**
         * The icon for this component
         */
        HICON m_icon;

    };

}

#endif // !defined(TrainRadioGroupComponent_H)
