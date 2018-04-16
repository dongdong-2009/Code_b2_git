/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h $
 * @author:  
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This is the common interface for the CEntityDetailsAreaDlg strategy. These
 * strategies will define the behaviour of the CEntityDetailsAreaDlg for different
 * Resource types. The only operation thas has to be explicitly implemented on all
 * strategies is the Initialise operation. All other operations can either be
 * overloaded with an empty stub, or the interface method itself may be stubbed.
 */

#if !defined(AFX_IENTITYDETAILSAREASTRATEGY_H__0F75A700_0606_497E_90A6_11751B365899__INCLUDED_)
#define AFX_IENTITYDETAILSAREASTRATEGY_H__0F75A700_0606_497E_90A6_11751B365899__INCLUDED_


#include "bus/radio/radio_bus/src/RadioTypes.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEntityDetailsAreaDlg;

class IEntityDetailsAreaStrategy  
{
	public:
		
		virtual ~IEntityDetailsAreaStrategy() {};

		/**
		 * Initialize the dialog by showing/hiding and populating any controls based on
		 * the entity Type.
		 */
		virtual void initialise(CEntityDetailsAreaDlg* pentityDetailsArea) =0;
		/**
		 * Perform the Edit function of the Resource type.
		 */
		virtual void edit() =0;
		/**
		 * Perform the Delete function of the Resource type. 
		 */
		virtual void deleteResource() =0;
		/**
		 * Perform the Save function of the Resource type.
		 */
		virtual void save() =0;
		/**
		 * Perform the Cancel function of the Resource type.
		 */
		virtual void cancel() =0;
		/**
		 * Perform the Drop Files function of the Resource type.
		 */
		virtual void OnDropFiles(RadioResource& resource) =0;

		
	public:

		CEntityDetailsAreaDlg *m_CEntityDetailsAreaDlg;

};

#endif // !defined(AFX_IENTITYDETAILSAREASTRATEGY_H__0F75A700_0606_497E_90A6_11751B365899__INCLUDED_)
