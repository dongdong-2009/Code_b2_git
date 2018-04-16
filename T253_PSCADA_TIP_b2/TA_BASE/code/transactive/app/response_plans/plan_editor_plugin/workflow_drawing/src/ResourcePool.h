#pragma once

#include <map>
#include <string>

class CBitmap;

typedef std::pair<CBitmap*, int> ResourceUsage;
typedef std::map<unsigned int, ResourceUsage> IdResourceUsageMap;
typedef std::map<std::string, ResourceUsage> StrResourceUsageMap;

class ResourcePool
{
public:
	static ResourcePool& instance();
	static void release();
	~ResourcePool();

    CBitmap& loadBitmap( const unsigned int unResourceId );
	CBitmap& loadBitmap( const std::string& strFile );
    void releaseBitmap( const unsigned int unResourceId );
    void releaseBitmap( const std::string& strFile );

private:
	ResourcePool();

private:
	static ResourcePool* s_pInstance;
	IdResourceUsageMap m_mapIdResources;
    StrResourceUsageMap m_mapStrResources;
    CBitmap* m_pBlankBitmap;
};

