#pragma once

#include "app/cctv/video_display_manager/src/ILoadStationDiagramService.h"
#include <string>
namespace TA_IRS_App
{

class GwxServiceMock :public ILoadStationDiagramService
{
public:
	GwxServiceMock(void);
	int loadStationDiagram ( const std::string& strFileFullName, const CRect& rtPosition, HWND hParent ) 
	{
		return true;
	}
	 void closeSchematic()
	 {
	 }

    void refreshSchematic()
	{
	}
public:
	~GwxServiceMock(void);
};
}
