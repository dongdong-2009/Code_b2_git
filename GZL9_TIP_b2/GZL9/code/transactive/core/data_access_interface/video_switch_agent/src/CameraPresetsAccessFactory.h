#if !defined(CameraPresetsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
#define CameraPresetsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/CameraPresetsAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * CameraPresetsAccessFactory is a singleton that is used to generate and retrieve 
  * ICameraPresets objects.
  */

#include <string>

namespace TA_IRS_Core
{
    class ICameraPresets;    // Forward declaration.

    class CameraPresetsAccessFactory
    {
    public:

        /**
         * ~CameraPresetsAccessFactory
         *
         * Standard destructor.
         */

        virtual ~CameraPresetsAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      CameraPresetsAccessFactory&
         *              A reference to an instance of a CameraPresetsAccessFactory object.
         */

        static CameraPresetsAccessFactory& getInstance()
        {
            static CameraPresetsAccessFactory fact;
            return fact;
        }

        /**
         * getCameraPresets
         *
         * Retrieves the presets for the given Camera.
         *
         * @return      ICameraPresets*
         *              A pointer to an object conforming to the ICameraPresets interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @param       unsigned long cameraKey
         *              The entity key of the Camera.
         */

        ICameraPresets* getCameraPresets( unsigned long cameraKey );

    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        CameraPresetsAccessFactory() {};
        CameraPresetsAccessFactory& operator=(const CameraPresetsAccessFactory &) {};
        CameraPresetsAccessFactory( const CameraPresetsAccessFactory& ) {};
    };

} // end namespace TA_IRS_Core

#endif // !defined(CameraPresetsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
