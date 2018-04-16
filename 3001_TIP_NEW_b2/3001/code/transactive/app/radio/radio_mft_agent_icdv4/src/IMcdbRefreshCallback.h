#pragma once

namespace ICDV4{
class IMcdbRefreshCallback
{
public:
virtual void onMcdbRefreshComplete()=0;
};
}