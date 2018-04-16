/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/trending/trend_viewer/src/TrendTreeCtrl.h $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// TrendTreeCtrl.h : header file
//


#if !defined(TRENDTREECTRL_H_INCLUDED)
#define TRENDTREECTRL_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/mfc_extensions.h"


namespace TA_Base_App
{
 
    /////////////////////////////////////////////////////////////////////////////
    // CTrendTreeCtrl
	class IItemStore;
    
    class CTrendTreeCtrl : public CObjectTreeCtrl
    {
    // Construction
    public:
 
		CTrendTreeCtrl(IItemStore* itemStore);
		virtual ~CTrendTreeCtrl();

    // Attributes
    public:

    // Operations
    public:  

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CTrendTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  	//}}AFX_VIRTUAL

    // Implementation
 	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CTrendTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

    private:

	    void loadTreeTrendItems();
	    DECLARE_MESSAGE_MAP()

        IItemStore* m_itemStore;
		// This is the entity type that will be at the root of our tree
        static const std::string ROOT_ENTITY_TYPE;   
		bool m_canInitialiseInPreSubclassWindow;
    };

} // namespace TA_Base_App

#endif // !defined(TRENDTREECTRL_H_INCLUDED)
