#ifndef __IPOWER_SECTION_STATUS_OBSERVER_H_INCLUDED__
#define __IPOWER_SECTION_STATUS_OBSERVER_H_INCLUDED__

namespace TA_IRS_Bus
{
	class IPowerSectionStatusObserver
	{
	public:
		/* 
		 * onPowerStatusUpdate
		 * @purpose: update data store power status
		 * @param:	sectionID - power section ID.
		 *			powerStatus 
		 *
		 * @return: none
		 */
		virtual void onPowerStatusUpdate(const unsigned short& sectionID, const unsigned char& powerStatus)=0;

	};
}
#endif //__IGENA_TOPIC_DATA_STORE_H_INCLUDED__