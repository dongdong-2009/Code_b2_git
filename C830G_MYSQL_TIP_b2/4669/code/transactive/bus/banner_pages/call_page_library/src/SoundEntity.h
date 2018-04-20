// SoundEntity.h: interface for the SoundEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDENTITY_H__ADDA7878_6865_4ECC_86BC_DBAB3E9226AB__INCLUDED_)
#define AFX_SOUNDENTITY_H__ADDA7878_6865_4ECC_86BC_DBAB3E9226AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>


namespace TA_IRS_Bus 
{
class SoundEntity  
{
public:
	SoundEntity()
		:m_sCatergory(""),
		m_iPriority(0)
	{

	}
	virtual ~SoundEntity()
	{

	}
	std::string getCatergory()
	{
		return m_sCatergory.c_str();
	}

	int getPriority()
	{
		return m_iPriority;
	}

	void setCatergory ( std::string catergory )
	{
		m_sCatergory = catergory.c_str() ;
	}

	void setPrority( int priority )
	{

		m_iPriority = priority;
	}
	
private:
	
	
	std::string m_sCatergory ;

	int m_iPriority ;
	

};
}

#endif // !defined(AFX_SOUNDENTITY_H__ADDA7878_6865_4ECC_86BC_DBAB3E9226AB__INCLUDED_)
