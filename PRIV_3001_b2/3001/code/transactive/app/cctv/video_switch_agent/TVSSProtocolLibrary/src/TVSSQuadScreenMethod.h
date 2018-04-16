#pragma once
#include<vector>

#include "TVSSMethod.h"
#include "TVSSProtocol.h"

namespace TA_IRS_App
{
	typedef std::vector<unsigned char> Camera;
	class TVSSQuadScreenMethod : public TVSSMethod
	{
	public:
		TVSSQuadScreenMethod(unsigned char monitorId, Camera data);

	   /**
	    * Use to set the Cameras from Data1 to Data4.
		* Data1=CameraW, Data2=CameraX, Data3=CameraY, Data4=CameraZ
		* @param setData
		*       setData is a vector containter for the 4 Cameras.
		*/
		void setCameraData(Camera setData);

	public:
		virtual ~TVSSQuadScreenMethod(void);
	private:
	   /**
        * m_cameraData is a vector container for the Data1,...,Data4.
		* Data1=CameraW, Data2=CameraX, Data3=CameraY, Data4=CameraZ
		*/
		Camera m_cameraData;
	};
};
