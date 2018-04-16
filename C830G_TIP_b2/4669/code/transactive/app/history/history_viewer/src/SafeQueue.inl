// SafeQueue.cpp: implementation of the SafeQueue Template class.

//

//////////////////////////////////////////////////////////////////////



#include "stdafx.h"

//#include "typeViewer.h"

#include "SafeQueue.h"
#include "Thread.h"

/*

#ifdef _DEBUG

#undef THIS_FILE

static char THIS_FILE[]=__FILE__;

#define new DEBUG_NEW

#endif



template class SafeQueue<unsigned long>;

template class SafeQueue<sign>;

*/



//Thread safe to push a Type on to the queue

template <class Type>

void SafeQueue<Type>::push(const Type& type)

{

	//Make it all thread safe

    TA_Core::TA_Synchronisation::ThreadGuard guard(*this);



	m_queue.push(type);

}





//This returns true if there is a Type in the queue - else false

//Populates the Type arg with the Type popped, assuming there is one to pop.

//If not it leaves the Type unchanged

template <class Type>

bool SafeQueue<Type>::pop(Type& type)

{

	//Make it all thread safe

	TA_Core::TA_Synchronisation::ThreadGuard guard(*this);



	if (m_queue.empty())

	{

		return false;

	}



	else

	{

		//Do the copy to avoid getting a reference to the object in the queue

		Type temptype = m_queue.front();

		type = temptype;

		m_queue.pop();

		return true;

	}



}



