#pragma once

namespace TA_IRS_App {
class IMcdbRefreshCallback
{
public:
virtual void onMcdbRefreshComplete(short mode = 0)=0;
};

}