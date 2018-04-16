/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/control_station/src/GraphWorxApplication.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *  
  * This class will communicate with GraphWorX. It will launch a single instance of GraphWorx which
  * will be terminated when the user logs out. This is different to the GraphWorXCommunicator in that
  * it does not ensure the GraphWorX instance continues to run and it immediately launches and 
  * displays it.
  */

#if !defined(GraphWorxApplication_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_)
#define GraphWorxApplication_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_

#include "app\system_control\control_station\src\IGraphWorxManager.h"
#include <string>
#include "core/threads/src/Thread.h"

// Forward declare. Header included in the cpp file.
class IGwx32;

class GraphWorxApplication
{
public:


	/**
    * Constructor
    */
    GraphWorxApplication(IGraphWorxManager::GraphWorXInstance instance);


	/**
    * Destructor
    */
	virtual ~GraphWorxApplication();


private:
    // Made private as these are not needed.
    GraphWorxApplication( const GraphWorxApplication& theGraphWorxApplication);
    GraphWorxApplication& operator=(const GraphWorxApplication& rhs);


    /** 
     * runGraphWorx
     *
     * This will actually kick off an instance of GraphWorX, load the display and position it.
     */
    void runGraphWorx();


    /** 
     * terminateGraphWorx
     *
     * This will nicely terminate the instance of GraphWorX.
     */
    void terminateGraphWorx();


    // This holds each GraphWorX window, configuration information for it and whether
    // it is running or not.
    IGraphWorxManager::GraphWorXInstance m_graphWorXWindow;
};

#endif // !defined(GraphWorxApplication_CDF2CE12_706C_411a_B627_B2DFBC17F7D7__INCLUDED_)
