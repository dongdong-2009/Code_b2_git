// INotifyFrameRepetition.h: interface for the INotifyFrameRepetition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INOTIFYFRAMEREPETITION_H__9BB19FEE_8230_4D5C_9131_2E5A0AFB6139__INCLUDED_)
#define AFX_INOTIFYFRAMEREPETITION_H__9BB19FEE_8230_4D5C_9131_2E5A0AFB6139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class INotifyFrameRepetition  
{
public:
	virtual void notifyFrameRepeat (std::vector <long> vctrFrameRepeat) = 0;

};

#endif // !defined(AFX_INOTIFYFRAMEREPETITION_H__9BB19FEE_8230_4D5C_9131_2E5A0AFB6139__INCLUDED_)
