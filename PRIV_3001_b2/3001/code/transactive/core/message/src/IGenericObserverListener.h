/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/message/src/IGenericObserverListener.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  */

#if !defined(IGenericObserverListener_42CFD151_6256_4ef7_B9D2_76A1D56195F3__INCLUDED_)
#define IGenericObserverListener_42CFD151_6256_4ef7_B9D2_76A1D56195F3__INCLUDED_

/**
 * This interface is used by the GenericSubscriverPublisher abstract class to
 * contain a list of various ObserverListeners.
 * @author F. Stuart
 * @version 1.0
 * @created 19-May-2005 06:46:29 PM
 */
class IGenericObserverListener
{
public:
    virtual ~IGenericObserverListener() {}
};
#endif // !defined(IGenericObserverListener_42CFD151_6256_4ef7_B9D2_76A1D56195F3__INCLUDED_)