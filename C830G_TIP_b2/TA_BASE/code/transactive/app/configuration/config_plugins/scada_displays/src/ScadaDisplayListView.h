/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/configuration/config_plugins/scada_displays/src/ScadaDisplayListView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all Scada displays available to be configured. This view must communicate
  * with the ScadaDisplayDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_ScadaDisplayListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_ScadaDisplayListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class ScadaDisplayDetailView;
    class ScadaDisplayDatabaseAccessor;

    class ScadaDisplayListView : public AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    ScadaDisplayListView();


	    DECLARE_DYNCREATE(ScadaDisplayListView)

	    virtual ~ScadaDisplayListView();

    // Operations
    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ScadaDisplayListView)
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(ScadaDisplayListView)
    	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon()
        {
            return m_componentIcon;
        }


        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::SCADA_DISPLAY;
        }

    private:
        HICON m_componentIcon;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ScadaDisplayListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
