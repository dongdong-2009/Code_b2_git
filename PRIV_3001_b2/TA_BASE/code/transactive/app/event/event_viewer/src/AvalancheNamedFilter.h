/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/event/event_viewer/src/AvalancheNamedFilter.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class implements the dynamic filtering ability required to filter
  * by the alarm ID of the currently selected event.
  *
  */

#if !defined(_AVALANCHE_NAMED_FILTER_H_)
#define _AVALANCHE_NAMED_FILTER_H_

#include "core/data_access_interface/src/NamedFilter.h"

class Filter;

class AvalancheNamedFilter : public TA_Base_Core::NamedFilter
{
public:
    // Static constants for storing static filter names.
    static const std::string NAMEDFILTER_AVALANCHE;

    /**
     * Constructor
     *
     * @param filter The filter that will be queried to determine the
     *               state of this filter.
     */
    AvalancheNamedFilter( Filter* filter );

    /**
     * Destructor
     */
    virtual ~AvalancheNamedFilter();

    /**
     * clone
     *
     * This method is used by static filters to duplicate themselves. It is
     * the responsibility of the caller to delete the returned object.
     *
     * @return NamedFilter* The new filter.
     */
    virtual TA_Base_Core::NamedFilter* clone();

private:
    Filter* m_filter;
};

#endif // !defined(_AVALANCHE_NAMED_FILTER_H_)