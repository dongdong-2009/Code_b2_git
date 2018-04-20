// GRC3DataPointSetComplexStep.h: interface for the GRC3DataPointSetComplexStep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRC3DATAPOINTSETCOMPLEXSTEP_H__6DFF67B5_501A_4878_AECB_A71D2976ADCE__INCLUDED_)
#define AFX_GRC3DATAPOINTSETCOMPLEXSTEP_H__6DFF67B5_501A_4878_AECB_A71D2976ADCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

namespace TA_Base_App
{
	class GRC3DataPointSetComplexStep  : public ActiveStep 
	{
		// Public methods
		public:
			GRC3DataPointSetComplexStep(const TA_Base_Core::StepDetail& stepDetail,
								 		 IActivePlanFlow& flowControl);
			virtual ~GRC3DataPointSetComplexStep();

			void addChildStep(ActiveDataPointSetStep *pDataPointSetStep);

		protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

			bool doSetDataPoint( );

			void startAllSubTaskThread();
			void stopAllSubTaskThread();
	private:

		typedef std::vector<GRCType3SubTask*> DataPointSetSubTaskVector;
		DataPointSetSubTaskVector m_vtDataPointSetSubTask;

        // The lock for protecting access to the sessions list.


		private:
			GRC3DataPointSetComplexStep& operator=( const GRC3DataPointSetComplexStep& );
			GRC3DataPointSetComplexStep( const GRC3DataPointSetComplexStep& );

			IActivePlanFlow& m_flowControl;

			std::vector<ActiveDataPointSetStep*> m_vtActiveDataPointSetStep;

	};
}

#endif // !defined(AFX_GRC3DATAPOINTSETCOMPLEXSTEP_H__6DFF67B5_501A_4878_AECB_A71D2976ADCE__INCLUDED_)
