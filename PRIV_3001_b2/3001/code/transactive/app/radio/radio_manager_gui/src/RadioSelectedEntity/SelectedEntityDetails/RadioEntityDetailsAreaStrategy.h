/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/RadioEntityDetailsAreaStrategy.h $
 * @author:  Alexis Laredo
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_RADIOENTITYDETAILSAREASTRATEGY_H__536E20A9_D28C_4228_BB41_CA60D6F35B2E__INCLUDED_)
#define AFX_RADIOENTITYDETAILSAREASTRATEGY_H__536E20A9_D28C_4228_BB41_CA60D6F35B2E__INCLUDED_


#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RadioEntityDetailsAreaStrategy : public IEntityDetailsAreaStrategy 
{
	public:
		RadioEntityDetailsAreaStrategy();
		virtual ~RadioEntityDetailsAreaStrategy();


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
        RadioEntityDetailsAreaStrategy (RadioEntityDetailsAreaStrategy& radioObj);
        void operator=(const RadioEntityDetailsAreaStrategy& radioObj);
};

#endif // !defined(AFX_RADIOENTITYDETAILSAREASTRATEGY_H__536E20A9_D28C_4228_BB41_CA60D6F35B2E__INCLUDED_)
