/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/MultiSiteEntityDetailsAreaStrategy.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
// MultiSiteEntityDetailsAreaStrategy.h: interface for the MultiSiteEntityDetailsAreaStrategy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISITEENTITYDETAILSAREASTRATEGY_H__A2D0A968_ADB1_4CF0_B022_914DD2D3FAF5__INCLUDED_)
#define AFX_MULTISITEENTITYDETAILSAREASTRATEGY_H__A2D0A968_ADB1_4CF0_B022_914DD2D3FAF5__INCLUDED_


#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MultiSiteEntityDetailsAreaStrategy : public IEntityDetailsAreaStrategy  
{
	public:

		MultiSiteEntityDetailsAreaStrategy();
		virtual ~MultiSiteEntityDetailsAreaStrategy();

	public:
		/**
		 * Initialize the dialog by showing/hiding and populating any controls based on
		 * the entity Type.
		 */
		virtual void initialise(CEntityDetailsAreaDlg* pentityDetailsArea);
		/**
		 * Perform the Delete function of the Resource type. 
		 */
		virtual void deleteResource();
		/**
		 * Perform the Edit function of the Resource type.
		 */
		virtual void edit();
		/**
		 * Perform the Save function of the Resource type.
		 */
		virtual void save();
		/**
		 * Perform the Cancel function of the Resource type.
		 */
		virtual void cancel();
		/**
		 * Perform the Drop Files function of the Resource type.
		 */
		virtual void OnDropFiles(RadioResource& resource);


	private:
        //disable method
        MultiSiteEntityDetailsAreaStrategy (MultiSiteEntityDetailsAreaStrategy& radioObj);
        void operator=(const MultiSiteEntityDetailsAreaStrategy& radioObj);


	private:
		/**
		 * Members added to ListBox on Edit
		 */
		std::vector<RadioResource> m_dropMembers;

};

#endif // !defined(AFX_MULTISITEENTITYDETAILSAREASTRATEGY_H__A2D0A968_ADB1_4CF0_B022_914DD2D3FAF5__INCLUDED_)
