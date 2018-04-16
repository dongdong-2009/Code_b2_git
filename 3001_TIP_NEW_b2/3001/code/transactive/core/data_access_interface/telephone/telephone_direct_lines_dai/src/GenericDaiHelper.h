/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/GenericDaiHelper.h $
  * @author Jade Lee
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2012/12/28 18:54:08 $
  * Last modified by: $Author: weikun.lin $
  * 
  * Provides common functionality between Data Access Interface helper
  *     classes (to enforce consistency between classes)
  *
  */

#ifndef _GENERIC_DAI_HELPER_HEADER_
#define _GENERIC_DAI_HELPER_HEADER_

#include <vector>
#include <string>

#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/DatabaseQueryHelper.h"

class TA_Base_Core::IData;

template <class DerivedClass>
class GenericDaiHelper
{
public:

    virtual ~GenericDaiHelper() {}

    /**
     * getBasicQueryData
     *
     * Provides default inputs for a query statement for this object
     *
     * NOTE: Classes using this template must provide a static implementation
     *  of this function
     *
     * @return a query structure that can be used to retrieve all required
     *          elements from the table
     *         
     */
    DatabaseQueryHelper::QueryData getBasicQueryData();

protected:     
    
    /**
     * reload
     *
     * This method reloads the data from the database. It is called when a get... method
     * is called and the data state is not valid.         
     *
     * @param queryData the query we should use to retrieve the information from
     *          the database, typically received from getBasicQueryData
     *
     * @pre:    This PaZone was initially loaded from the database
     *          or writePaZoneData() has already been called (meaning
     *          getKeyDirect doesn't throw an exception, and returns nonzero value
     *
     * @throws DatabaseException 
     *         - if there is a problem establishing a connection with the database.
     *         - if an error is encountered while retrieving data.
     * @throws DataException
     *         - if the data cannot be converted to the required format 
     *           (e.g. the PA Location Key should be an unsigned long)
     *         - NO_VALUE if the PaZone record cannot be found for the helper 
     *           constructed with a key.
     *         - NOT_UNIQUE if the primary key returns multiple records 
     *
     */
    void reload(const DatabaseQueryHelper::QueryData& queryData);

    /**
     * reload
     *
     * Uses the template class static function to obtain the query data
     *   and passes this onto main reload(const QueryData&) function
     *
     */
    void reload();

    /**
     * reloadUsing
     *
     * Fills out the local members with data contained in the input data object
     *
     * @param row the row to query in the data object
     *
     * @param data the IData interface that should have been obtained using the 
     *              getBasicQueryData function
     *         
     * @throws DatabaseException 
     *         - if there is a problem establishing a connection with the database.
     *         - if an error is encountered while retrieving data.
     * @throws DataException
     *         - if the data cannot be converted to the required format 
     *         - NO_VALUE if the record cannot be found for the helper 
     *           constructed with a key.
     *         - NOT_UNIQUE if the primary key returns multiple records 
     *
     */
    virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data) = 0;

    /**
     * getKeyDirect
     *
     * @return the m_key member directly, this is for internal
     *      use only
     *
     * @exception ValueNotSetException if key unavailable
     *
     */
    virtual unsigned long getKeyDirect() const = 0;
};

#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/GenericDaiHelper.inl"

#endif // _GENERIC_DAI_HELPER_HEADER_