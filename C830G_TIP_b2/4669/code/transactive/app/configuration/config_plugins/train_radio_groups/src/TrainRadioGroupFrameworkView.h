/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupFrameworkView.h $
  * @author:  oliverk
  * @version: $Revision: #1 $
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This view contains a splitter. It then loads the left side of the splitter as a list of train location
  * and the right side as a view for the user to configure train radio groups details.
  */

#if !defined(TrainRadioGroupFrameworkView_H)
#define TrainRadioGroupFrameworkView_H

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractFrameworkView.h"

namespace TA_IRS_App
{
    /**
     * @author oliverk
     * @version 1.0
     * @created 15-Jan-2008 12:51:27 PM
     */

    class TrainRadioGroupFrameworkView : public TA_Base_App::AbstractFrameworkView
    {

    protected:

        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
        TrainRadioGroupFrameworkView();


        DECLARE_DYNCREATE( TrainRadioGroupFrameworkView )


        /**
         * Destructor
         */
        virtual ~TrainRadioGroupFrameworkView();


#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump( CDumpContext& dc ) const;
#endif


        DECLARE_MESSAGE_MAP()

        
        /**
         * getRuntimeClassForListView
         *
         * This returns the runtime class for the list view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
        virtual CRuntimeClass* getRuntimeClassForListView();

       
        /**
         * getRuntimeClassForDetailView
         *
         * This returns the runtime class for the detail view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
        virtual CRuntimeClass* getRuntimeClassForDetailView();

    };

}

#endif // !defined(TrainRadioGroupFrameworkView_H)
