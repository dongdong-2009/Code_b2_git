/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/MultiselectEntityDetailsAreaStrategy.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
// MultiselectEntityDetailsAreaStrategy.h: interface for the MultiselectEntityDetailsAreaStrategy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISELECTENTITYDETAILSAREASTRATEGY_H__46EAD192_5098_44BE_A2C9_A119A921C82E__INCLUDED_)
#define AFX_MULTISELECTENTITYDETAILSAREASTRATEGY_H__46EAD192_5098_44BE_A2C9_A119A921C82E__INCLUDED_


#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MultiselectEntityDetailsAreaStrategy : public IEntityDetailsAreaStrategy  
{
	public:

		MultiselectEntityDetailsAreaStrategy();
		virtual ~MultiselectEntityDetailsAreaStrategy();


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
        MultiselectEntityDetailsAreaStrategy (MultiselectEntityDetailsAreaStrategy& radioObj);
        void operator=(const MultiselectEntityDetailsAreaStrategy& radioObj);


	private:
		/**
		 * Members added to ListBox on Edit
		 */
		std::vector<RadioResource> m_dropMembers;
};

#endif // !defined(AFX_MULTISELECTENTITYDETAILSAREASTRATEGY_H__46EAD192_5098_44BE_A2C9_A119A921C82E__INCLUDED_)
