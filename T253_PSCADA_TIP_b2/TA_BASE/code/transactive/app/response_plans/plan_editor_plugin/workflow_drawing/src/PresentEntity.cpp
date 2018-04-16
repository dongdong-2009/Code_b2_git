#include "stdafx.h"
#include "ResourcePool.h"
#include "PresentEntity.h"


PresentEntity::PresentEntity( const unsigned int unResourceId ) :
m_nWidth( 0 ),
m_nHeight( 0 ),
m_unResourceId( unResourceId ),
m_strResourcePath( "" ),
m_refResource( ResourcePool::instance().loadBitmap( unResourceId ))
{
	loadResource();
}

PresentEntity::PresentEntity( const std::string& strResource ) :
m_nWidth( 0 ),
m_nHeight( 0 ),
m_unResourceId( 0u ),
m_strResourcePath( strResource.c_str() ),
m_refResource( ResourcePool::instance().loadBitmap( strResource ))
{
    loadResource();
}

PresentEntity::~PresentEntity()
{
	releaseResource();
}

void PresentEntity::loadResource()
{
    if ( NULL != m_refResource.GetSafeHandle() )
    {
        BITMAP bmpInfo;
        m_refResource.GetBitmap( &bmpInfo );

        m_nHeight = bmpInfo.bmHeight;
        m_nWidth = bmpInfo.bmWidth;
    }
}

void PresentEntity::releaseResource()
{
    if ( m_strResourcePath.empty() )
    {
        ResourcePool::instance().releaseBitmap( m_unResourceId );
    }
    else
    {
        ResourcePool::instance().releaseBitmap( m_strResourcePath );
    }
}

const int PresentEntity::width() const
{
	return m_nWidth;
}

const int PresentEntity::height() const
{
	return m_nHeight;
}

CBitmap& PresentEntity::getResource() const
{
    return m_refResource;
}
