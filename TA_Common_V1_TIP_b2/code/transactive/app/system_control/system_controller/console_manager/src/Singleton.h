#pragma once

template <typename T>
struct Singleton
{
    static T& getInstance()
    {
        return *ACE_Unmanaged_Singleton<T, ACE_Null_Mutex>::instance();
    }
};
