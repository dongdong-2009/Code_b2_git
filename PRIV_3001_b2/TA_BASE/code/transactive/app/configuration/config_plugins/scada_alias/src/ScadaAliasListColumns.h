/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/TA_BASE/transactive/app/configuration/config_plugins/scada_alias/src/ScadaAliasListColumns.h $
  * @author Wu Mintao
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma once

#include <map>
#include "app/configuration/config_plugins/config_plugin_helper/src/ListColumnsBase.h"
#include "app/configuration/config_plugins/scada_alias/src/ScadaAliasDatabaseAccessor.h"
class ScadaAliasListColumns 
:
public ListColumnsBase<TA_Base_App::ScadaAliasDatabaseAccessor::ScadaAliasRecord, ScadaAliasListColumns>
{
public:

    ScadaAliasListColumns();

    // getAssociatedEquipmentEntities() isn't covered in the columns
    enum EColumn
    {
        COL_OPC_ALIAS,                  //IConfigOpcTagAlias::getOpcAlias()
        COL_ENTITY_NAME,                // IConfigOpcTagAlias::getEntityName()
        COL_ATTRIBUTE,           // IConfigOpcTagAlias::getAttribute()
        COL_MAX        
    };    

    static DWORD getDefaultSortColumn() { return COL_OPC_ALIAS; }

    /**
     * getFieldFromRecord
     *
     * @return a string representing the specific field of this record corresponding to the input row
     * @param column the column defining which field we want to extract from the record
     * @param details the record for which the reference to a member is returned
     */
    static const CString getFieldFromRecord(DWORD column, const TA_Base_App::ScadaAliasDatabaseAccessor::ScadaAliasRecord& details);
};
