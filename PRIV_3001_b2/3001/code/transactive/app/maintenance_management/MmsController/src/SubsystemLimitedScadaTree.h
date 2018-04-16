/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/maintenance_management/MmsController/src/SubsystemLimitedScadaTree.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Defines a specialization of the scada tree that only shows
  * up to subsystem level (and optionally only allows selected of subsystem datanodes)
  *
  */

#pragma once

#include "MmsEntityFactory.h"
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"

namespace TA_Base_Core
{
    class DataNodeEntityData;
};

namespace TA_IRS_App
{

    class SubsystemLimitedScadaTree : public TA_Base_Bus::CScadaTreeCtrl
    {
    public:

        /**
         * @param onlyAllowSubsystemSelection true to suppress selection
         *      of anything but subsystem level zones
         */
        SubsystemLimitedScadaTree(bool onlyAllowSubsystemSelection);
        
        virtual ~SubsystemLimitedScadaTree();
        
        /**
         * @return true if there is one or more subsystems selected
         */
        bool isSubsystemSelected();
    
        /**
         * @return an array of selected subsystem entity objects
         *      Anything selected that is not a subsystem will not be included in result
         */
        std::vector<TA_Base_Core::DataNodeEntityData*> getSelectedSubsystems();

	protected:

		// Generated message map functions
		//{{AFX_MSG(CMmsControllerDlg)
		afx_msg void OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg BOOL OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG
		DECLARE_MESSAGE_MAP()

    private:

        /**
         * @return pointer to the entity defined by the input tree item
         *      or null if unable to determine entity type
         */
        TA_Base_Core::IEntityData* getEntityFromTreeItem(const TV_ITEM& tvItem);

        TA_IRS_App::MmsEntityFactory m_mmsEntityFactory;

        bool m_onlyAllowSubsystemSelection;
    };

}