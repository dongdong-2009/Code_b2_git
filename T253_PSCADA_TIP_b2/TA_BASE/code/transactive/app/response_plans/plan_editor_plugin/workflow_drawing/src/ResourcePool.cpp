#include "stdafx.h"
#include "ResourcePool.h"

ResourcePool* ResourcePool::s_pInstance = NULL;

ResourcePool::ResourcePool() :
m_mapIdResources(),
m_pBlankBitmap( NULL )
{
    if ( NULL == m_pBlankBitmap )
    {
        m_pBlankBitmap = new CBitmap();
        unsigned int unRGB = 0x000000u;
        m_pBlankBitmap->CreateBitmap( 1, 1, 0u, 1, &unRGB );
    }
}

ResourcePool::~ResourcePool()
{
    if ( NULL != m_pBlankBitmap )
    {
        m_pBlankBitmap->DeleteObject();
        delete m_pBlankBitmap;
        m_pBlankBitmap = NULL;
    }
}

ResourcePool& ResourcePool::instance()
{
	if (NULL == s_pInstance)
	{
		s_pInstance = new ResourcePool();
	}

	return *s_pInstance;
}

void ResourcePool::release()
{
	if (NULL != s_pInstance)
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}
}

CBitmap& ResourcePool::loadBitmap(const unsigned int unResourceId)
{
	IdResourceUsageMap::iterator itFound = m_mapIdResources.find(unResourceId);

	if (m_mapIdResources.end() == itFound)
	{
		itFound = m_mapIdResources.insert(IdResourceUsageMap::value_type(unResourceId, ResourceUsage(new CBitmap(), 0))).first;
	}

	CBitmap* pResource = NULL;

	if (m_mapIdResources.end() != itFound)
	{
		if (itFound->second.second == 0)
		{
			itFound->second.first->LoadBitmap(unResourceId);
		}

		pResource = itFound->second.first;

		// Increase usage
		itFound->second.second += 1;
	}

	return *pResource;
}

CBitmap& ResourcePool::loadBitmap( const std::string& strFile )
{
    HBITMAP hLoaded = static_cast<HBITMAP>(::LoadImage( NULL, strFile.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ));

    if ( NULL == hLoaded )
    {
        return *m_pBlankBitmap;
    }

    StrResourceUsageMap::iterator itFound = m_mapStrResources.find( strFile );

    if ( m_mapStrResources.end() == itFound )
    {
        itFound = m_mapStrResources.insert( StrResourceUsageMap::value_type( strFile, ResourceUsage(new CBitmap(), 0))).first;
    }

    CBitmap* pResource = NULL;

    if ( m_mapStrResources.end() != itFound )
    {
        if ( itFound->second.second == 0 )
        {
            itFound->second.first->Attach( hLoaded );
        }

        pResource = itFound->second.first;

        // Increase usage
        itFound->second.second += 1;
    }

    return *pResource;
}

void ResourcePool::releaseBitmap(const unsigned int unResourceId)
{
	IdResourceUsageMap::iterator itFound = m_mapIdResources.find( unResourceId );

	if (m_mapIdResources.end() != itFound)
	{
		// Decrease usage
		itFound->second.second -= 1;

		if (itFound->second.second == 0)
		{
			itFound->second.first->DeleteObject();

            delete itFound->second.first;

            m_mapIdResources.erase( itFound );
		}
	}
}

void ResourcePool::releaseBitmap( const std::string& strFile )
{
    StrResourceUsageMap::iterator itFound = m_mapStrResources.find( strFile );

    if ( m_mapStrResources.end() != itFound )
    {
        // Decrease usage
        itFound->second.second -= 1;

        if ( itFound->second.second == 0 )
        {
            itFound->second.first->DeleteObject();

            delete itFound->second.first;

            m_mapStrResources.erase( itFound );
        }
    }
}
