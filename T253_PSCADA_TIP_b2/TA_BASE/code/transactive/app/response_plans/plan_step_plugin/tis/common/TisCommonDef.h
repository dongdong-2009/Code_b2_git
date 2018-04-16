#if !defined(AFX_TISCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
#define AFX_TISCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>


namespace TA_Base_App
{

	struct TISMessage
	{
		unsigned long libraryVersion;
		int librarySection;
		unsigned short messageId;
		TISMessage() : libraryVersion(0), librarySection(0), messageId(0)
		{
		}
	};
}

#endif //AFX_TISCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_