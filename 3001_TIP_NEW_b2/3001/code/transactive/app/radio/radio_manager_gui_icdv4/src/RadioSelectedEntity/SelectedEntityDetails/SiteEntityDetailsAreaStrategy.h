/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/SiteEntityDetailsAreaStrategy.h $
 * @author:  Alexis Laredo
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_SITEENTITYDETAILSAREASTRATEGY_H__D3622C67_4FF0_4674_A74F_8AAAFA3587FE__INCLUDED_)
#define AFX_SITEENTITYDETAILSAREASTRATEGY_H__D3622C67_4FF0_4674_A74F_8AAAFA3587FE__INCLUDED_


#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SiteEntityDetailsAreaStrategy : public IEntityDetailsAreaStrategy 
{
	public:
		SiteEntityDetailsAreaStrategy();
		virtual ~SiteEntityDetailsAreaStrategy();


	public:
		/**
		 * Initialize the dialog by showing/hiding and populating any controls based on
		 * the entity Type.
		 */
		virtual void initialise(CEntityDetailsAreaDlg* pentityDetailsArea);
		/**
		 * Perform the Delete function of the Resource type. 
		 */
		virtual void deleteResource() {};
		/**
		 * Perform the Edit function of the Resource type.
		 */
		virtual void edit() {};
		/**
		 * Perform the Save function of the Resource type.
		 */
		virtual void save() {};
		/**
		 * Perform the Cancel function of the Resource type.
		 */
		virtual void cancel() {};
		/**
		 * Perform the Drop Files function of the Resource type.
		 */
		virtual void OnDropFiles(RadioResource& resource) {};


	private:
        //disable method
        SiteEntityDetailsAreaStrategy (SiteEntityDetailsAreaStrategy& radioObj);
        void operator=(const SiteEntityDetailsAreaStrategy& radioObj);
};

#endif // !defined(AFX_SITEENTITYDETAILSAREASTRATEGY_H__D3622C67_4FF0_4674_A74F_8AAAFA3587FE__INCLUDED_)
