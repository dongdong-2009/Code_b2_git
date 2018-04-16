#pragma once

namespace TA_IRS_App
{
class IPTZControlView
{
public:
	IPTZControlView(void){}
	virtual unsigned short getPanSpeed () =0;
	virtual unsigned short getTiltSpeed() =0;
    virtual void enablePTZButton ( BOOL enabled ) = 0;
	virtual ~IPTZControlView(void){}
};
}
