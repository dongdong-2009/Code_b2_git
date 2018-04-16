#pragma once

#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

namespace TA_IRS_App
{
class TVSSEquipmentQueryMethod : public TVSSMethod 
{
public:
	TVSSEquipmentQueryMethod();
public:
	virtual ~TVSSEquipmentQueryMethod(void);
} ;
} ;