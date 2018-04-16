/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/DynamicRegroupEntityDetailsAreaStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Constructs the appropriate IEntityDetailsAreaStrategy type.
 */


#if !defined(AFX_DYNAMICREGROUPENTITYDETAILSAREASTRATEGY_H__761B5742_62AF_476A_9AB0_6D381A1843EC__INCLUDED_)
#define AFX_DYNAMICREGROUPENTITYDETAILSAREASTRATEGY_H__761B5742_62AF_476A_9AB0_6D381A1843EC__INCLUDED_


#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/GroupEntityStrategy.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DynamicRegroupEntityDetailsAreaStrategy  : public IEntityDetailsAreaStrategy,
												 public GroupEntityStrategy
{
	public:

		DynamicRegroupEntityDetailsAreaStrategy();
		virtual ~DynamicRegroupEntityDetailsAreaStrategy();

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
        DynamicRegroupEntityDetailsAreaStrategy (DynamicRegroupEntityDetailsAreaStrategy& radioObj);
        void operator=(const DynamicRegroupEntityDetailsAreaStrategy& radioObj);


	private:
		/**
		 * Members added to ListBox on Edit
		 */
		std::vector<RadioResource> m_dropMembers;

};

#endif // !defined(AFX_DYNAMICREGROUPENTITYDETAILSAREASTRATEGY_H__761B5742_62AF_476A_9AB0_6D381A1843EC__INCLUDED_)
