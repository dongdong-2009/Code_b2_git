/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/history_viewer/src/SafeQueue.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SafeQueue.h: interface for the SafeQueue class.

//

//////////////////////////////////////////////////////////////////////



#if !defined(AFX_SAFEQUEUE_H__7F347791_6C57_11D5_B19C_0050BAB0C99D__INCLUDED_)

#define AFX_SAFEQUEUE_H__7F347791_6C57_11D5_B19C_0050BAB0C99D__INCLUDED_



#include "ThreadGuard.h"

#include <queue>



#if _MSC_VER > 1000

#pragma once

#endif // _MSC_VER > 1000



//This class supplies a thread safe queue

template <class Type>

class SafeQueue : public TA_Core::TA_Synchronisation::ThreadLockable 

{

public:



	SafeQueue(){}



	virtual ~SafeQueue(){}



	//This returns true if there is a sign in the queue - else false

	//Populates the sign arg with the sign popped, assuming there is one to pop

	//If not it leaves the sign unchanged

	//This method is thread safe

	bool pop(Type& type);



	//Thread safe to push a sign on to the queue

	void push(const Type& type);



private:



	//The queue

	std::queue<Type> m_queue;

};





#include "SafeQueue.inl"



#endif // !defined(AFX_SAFEQUEUE_H__7F347791_6C57_11D5_B19C_0050BAB0C99D__INCLUDED_)





