/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/AttachMonitorAudioEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of AttachMonitorAudio Event Reader
  *
  */
#include "AttachMonitorAudioEventReader.h"

namespace TA_IRS_App
{

    AttachMonitorAudioEventReader::AttachMonitorAudioEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_ATTACHMONITORAUDIO,ATTACHMONITORAUDIO_EVENT_LENGTH)
    {
    }

    AttachMonitorAudioEventReader::~AttachMonitorAudioEventReader()
    {
    }

    unsigned long AttachMonitorAudioEventReader::getCallReference() const
    {
        return getParameterLong(CALL_REFERENCE_START);
    }

    unsigned long AttachMonitorAudioEventReader::getAudioReferenceA() const
    {
        return getParameterLong(AUDIO_REFERENCE_A_START);
    }

    unsigned long AttachMonitorAudioEventReader::getAudioReferenceB() const
    {
        return getParameterLong(AUDIO_REFERENCE_B_START);
    }

    const unsigned int AttachMonitorAudioEventReader::ATTACHMONITORAUDIO_EVENT_LENGTH   = 12;
    const unsigned int AttachMonitorAudioEventReader::CALL_REFERENCE_START              = 0;
    const unsigned int AttachMonitorAudioEventReader::AUDIO_REFERENCE_A_START           = 4;
    const unsigned int AttachMonitorAudioEventReader::AUDIO_REFERENCE_B_START           = 8;
} // namespace TA_IRS_App


