#pragma once

#include <string>

class CBitmap;
class CDC;

class PresentEntity
{
public:
    PresentEntity( const unsigned int unResourceId );
    PresentEntity( const std::string& strResource );
	~PresentEntity();

	void loadResource();
	void releaseResource();
    CBitmap& getResource() const;

	const int width() const;
    const int height() const;

private:
	int m_nWidth;
	int m_nHeight;
    unsigned int m_unResourceId;
    std::string m_strResourcePath;
	CBitmap& m_refResource;
};

