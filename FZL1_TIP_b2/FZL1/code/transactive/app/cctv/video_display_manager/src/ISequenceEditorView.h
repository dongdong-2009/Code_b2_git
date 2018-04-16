#pragma once

#include "app/cctv/video_display_manager/src/Sequence.h"

namespace TA_IRS_App
{
    class VideoDisplayManagerPresenter;
    class ISequenceEditorView
    {
        public:
            ISequenceEditorView(void) {}
            virtual ~ISequenceEditorView(void) {}
            virtual void Editsequence(Sequence* sequence, unsigned long stationID) = 0;
            virtual void SetPresenter(VideoDisplayManagerPresenter* presenter) = 0;
    };
}
