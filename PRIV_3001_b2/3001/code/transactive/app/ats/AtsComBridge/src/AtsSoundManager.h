/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsComBridge/src/AtsSoundManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

// AtsSoundManager.h: Definition of the AtsSoundManager class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSSOUNDMANAGER_H__6478E737_A390_4999_87D5_562CF3F03B68__INCLUDED_)
#define AFX_ATSSOUNDMANAGER_H__6478E737_A390_4999_87D5_562CF3F03B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols

#include "bus/sound/sound_client/src/ISoundObserver.h"

namespace TA_Base_Bus
{
	class SoundManagerProxy;
}

/////////////////////////////////////////////////////////////////////////////
// AtsSoundManager

class AtsSoundManager : 
	public IDispatchImpl<IAtsSoundManager, &IID_IAtsSoundManager, &LIBID_AtsComBridge>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<AtsSoundManager,&CLSID_AtsSoundManager>,
	public TA_Base_Bus::ISoundObserver
{
public:
	AtsSoundManager();
	virtual ~AtsSoundManager();
BEGIN_COM_MAP(AtsSoundManager)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IAtsSoundManager)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(AtsSoundManager) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_AtsSoundManager)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IAtsSoundManager
public:
	STDMETHOD(setAlarmSeverity)(long alarmSeverity);

protected:
	void initSoundInterface();
	TA_Base_Bus::SoundManagerProxy* m_soundInterface;

	//
	// ISoundObserver
	//
public:
	/** 
	  * handleCategorySilenced
	  *
	  * Called when a category of sounds has its silence state updated.
	  *
	  * @param category    The category of sounds that has been silenced or unsilenced.
	  * @param isSilenced  True if the category was silenced, and false if it was
	  *					   unsilenced.
	  *
	  */
	virtual void handleCategorySilenced(const std::string& category, bool isSilenced);


	/** 
	  * handleAllSilenced
	  *
	  * Called when all categories of sounds have been silenced or unsilenced.
	  *
	  * @param isSilenced  True if all sounds have been silenced, and false if they
	  *					   have been unsilenced.
	  */
	virtual void handleAllSilenced(bool isSilenced);

	// Initialisation of transactive stuff...
protected:
	void initialiseDebugUtil();
	void initialiseCorba(int portNum);
	void cleanupCorba();
};

#endif // !defined(AFX_ATSSOUNDMANAGER_H__6478E737_A390_4999_87D5_562CF3F03B68__INCLUDED_)
