/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioOperations/RadioOperationsObserverHelper.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This class will facilitate the managing and notifying of child windows
 * registered to receive notifications of a particular type.
 */

#if !defined(AFX_RADIOOPERATIONSOBSERVERHELPER_H__99C07F0E_2572_45E5_AAAE_986F0F60EC9F__INCLUDED_)
#define AFX_RADIOOPERATIONSOBSERVERHELPER_H__99C07F0E_2572_45E5_AAAE_986F0F60EC9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>


class RadioOperationsObserverHelper  
{
	public:
		RadioOperationsObserverHelper();
		virtual ~RadioOperationsObserverHelper();

	public:
		/**
		 * Add a child component as an observer of the specified user-defined message.
		 */
		void addChildObserver(CWnd* pObserver, unsigned long message);
		/**
		 * Remove a child component as an observer of the specified user-defined message.
		 */
		void removeChildObserver(CWnd* pObserver, unsigned long message);
		/**
		 * Posts the Windows message to all child windows subscriber to this message.
		 */
		void notifyChildWindows(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);


	private:
        //disable method
        RadioOperationsObserverHelper (const RadioOperationsObserverHelper& radioObj);
        void operator=(const RadioOperationsObserverHelper& radioObj);

	public:
		typedef std::vector<CWnd*> CWndVec;
		typedef std::map<unsigned long, boost::shared_ptr< CWndVec > > ChildCollection;

	private:
		/**
		 * This map will associate the user-defined windows messages with the list of
		 * child windows registered to receive the notifications. The key of the map will
		 * contain the message key and the type of the map will contain a pointer to the
		 * vector containing the list of child windows.
		 * 
		 * The C++ Boost shared_ptr class template (smart pointer) is used to manage the
		 * lifetime of the vectors containing the CWnd pointers.
		 */
		ChildCollection m_ChildObservers;

};

#endif // !defined(AFX_RADIOOPERATIONSOBSERVERHELPER_H__99C07F0E_2572_45E5_AAAE_986F0F60EC9F__INCLUDED_)
