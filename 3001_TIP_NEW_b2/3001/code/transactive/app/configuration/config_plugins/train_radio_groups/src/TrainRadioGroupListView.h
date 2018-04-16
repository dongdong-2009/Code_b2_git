/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupListView.h $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This view contains a list of all train radio grups available to be configured. This view must communicate
  * with the TrainRadioGroupsDetailView as this will show the details of the item selected in this view.
  */
#if !defined(TrainRadioGroupListView_H)
#define TrainRadioGroupListView_H


#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"


namespace TA_IRS_App
{

    /**
     * @author oliverk
     * @version 1.0
     * @created 15-Jan-2008 12:51:28 PM
     */

    class TrainRadioGroupListView : public TA_Base_App::AbstractListView
    {

    protected:


        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
        TrainRadioGroupListView();


        /**
         * Destructor
         */
        virtual ~TrainRadioGroupListView();


        DECLARE_DYNCREATE( TrainRadioGroupListView )


        // Operations

    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(TrainRadioGroupListView)

    protected:
        virtual void OnDraw( CDC* pDC );    // overridden to draw this view
        //}}AFX_VIRTUAL


        // Implementation

    protected:


#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump( CDumpContext& dc ) const;
#endif


        // Generated message map functions

    protected:
        //{{AFX_MSG(TrainRadioGroupListView)
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


    protected:


        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon();

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const;


    private:

        /**
         * The list icon
         */
        HICON m_componentIcon;

    };

}

#endif // !defined(TrainRadioGroupListView_H)
