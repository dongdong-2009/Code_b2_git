#pragma once

#include "TVSSMethod.h"
#include "TVSSProtocol.h"

namespace TA_IRS_App
{
class TVSSEquipmentQueryMethod : public TVSSMethod 
{
public:
	TVSSEquipmentQueryMethod(unsigned char tmf_server);
public:
	virtual ~TVSSEquipmentQueryMethod(void);
} ;
} ;