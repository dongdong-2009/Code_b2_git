#pragma once
#include "Sequence.h"
//#include "VideoDisplayManagerPresenter.h"

namespace TA_IRS_App
{
	class VideoDisplayManagerPresenter;
class ISequenceEditorView
{
public:
	ISequenceEditorView(void){}
    virtual void Editsequence ( Sequence * sequence,unsigned long stationID) = 0;
	virtual void SetPresenter ( VideoDisplayManagerPresenter * presenter ) =0;
		
public:
	~ISequenceEditorView(void) {}
};
}
