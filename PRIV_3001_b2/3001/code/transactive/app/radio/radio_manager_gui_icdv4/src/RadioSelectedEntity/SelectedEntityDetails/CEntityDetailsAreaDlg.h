/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef ENTITY_DETAILS_AREA_DLG_H_INCLUDED
#define ENTITY_DETAILS_AREA_DLG_H_INCLUDED

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaListBox.h"


#include <list>


class CEntityDetailsAreaDlg : public CDialog
{
	friend class IEntityDetailsAreaStrategy;
	friend class AnnouncementGroupEntityDetailsAreaStrategy;
	friend class DynamicRegroupEntityDetailsAreaStrategy;
	friend class MultiselectEntityDetailsAreaStrategy;
	friend class MultiSiteEntityDetailsAreaStrategy;
	friend class PatchGroupEntityDetailsAreaStrategy;
	friend class RadioEntityDetailsAreaStrategy;
	friend class SiteEntityDetailsAreaStrategy;
	friend class TalkGroupEntityDetailsAreaStrategy;
	friend class EmptyEntityDetailsAreaStrategy;

	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);

		CEntityDetailsAreaDlg(CWnd* pParent = NULL);

		virtual ~CEntityDetailsAreaDlg();
		/**
		 * If no RadioResource, display the default "empty" dialog.
		 */
		void initialise();

	
		//{{AFX_DATA(CEntityDetailsAreaDlg)
		enum { IDD = IDD_SELECTED_ENTITY };
		CStatic	m_staticBlank;
		CStatic	m_staticType;
		CStatic	m_staticID;
		CStatic	m_staticDesc;
		CStatic	m_staticText;
		CEdit	m_typeEL;
		CEntityDetailsAreaListBox	m_listLB;
		CEdit	m_idEL;
		CEdit	m_descriptionEL;
		CButton	m_edit;
		CButton	m_save;
		CButton	m_delete;
		//CButton m_deleteMember;
		CButton	m_cancel;
		CEdit   m_dgTalkgroupEL; 
		CStatic m_dgText; 
		//}}AFX_DATA


	//{{AFX_VIRTUAL(CEntityDetailsAreaDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
		//{{AFX_MSG(CEntityDetailsAreaDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnCancel();
		afx_msg void OnDelete();
		afx_msg void OnEdit();
		afx_msg void OnSave();
		afx_msg void OnDropFiles(HDROP hDropInfo);
		afx_msg LRESULT onNotifiedEntityBeingCreated(WPARAM pRadioResource = 0, LPARAM unused = 0);
		afx_msg LRESULT onNotifiedEntitySelectionChanged(WPARAM pRadioResource = 0, LPARAM unused = 0);
		afx_msg LRESULT onNotifiedConnectGroup(WPARAM pRadioResource = 0, LPARAM lParam = 0);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		afx_msg void OnDeleteMenu();
		afx_msg LRESULT onNotifiedEntityEmptySelection(WPARAM pRadioResourceType, LPARAM unused);
		HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	private:

		void enableControl(BOOL flag);

		void reInitialiseStrategy(RadioResourceType type);
		
		void refresh(unsigned long typeID, bool bOnInit = true); // false if invoked OnCancel
		
	// Attributes
	private:

		volatile IEntityDetailsAreaStrategy* m_pEntityDetailsAreaStrategy;
	
		bool m_bEditMode;

		bool m_bEditRights;

		//++Fixed TD#14320
		CRect m_listRect;
		CRect m_editRect;
		//--Fixed TD#14320

		CRect m_EditButtonRect;
		CRect m_SaveButtonRect;
		CRect m_DeleteButtonRect;
		//CRect m_DeleteMemberButtonRect;
		CRect m_CancelButtonRect;


	protected:
		/**
		 * The resource currently being displayed.
		 */
		RadioResource m_RadioResource;
		/**
		 * m_members is used to all sorts of concrete EntityDetailsAreaStrategy;
		 */
		std::vector<RadioResource> m_members;
		/**
		 * A handle to the parent window that will be responsible for managing
		 * notifications of events if required.
		 */
		CWnd* m_parent;
		
};
#endif
