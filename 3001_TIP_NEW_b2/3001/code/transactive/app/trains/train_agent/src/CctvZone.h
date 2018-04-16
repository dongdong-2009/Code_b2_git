/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(CctvZone_H)
#define CctvZone_H

namespace TA_IRS_App
{

    /**
     * This represents a CCTV zone for a train.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:23 PM
     */
    class CctvZone
    {

    public:

        /**
         * A CCTV zone can either be a normal, conflict, or pre conflict zone.
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:23 PM
         */
        enum ECctvZoneType
        {
            // Indicates a non conflict cctv zone
            NoConflict = 0,

            // Indicates a CCTV pre conflict zone
            PreConflict = 1,

            // Indicates a CCTV conflict zone
            Conflict = 2
        };


        /**
         * Default constructor which sets initial values
         */
        CctvZone();


        /**
         * Default constructor which sets initial values
         *
         * @param atsValue    The value received from ATS
         */
        CctvZone( double atsValue );
        
        
        /**
         * Default constructor which sets initial values
         *
         * @param atsValue    A stage number - this is used for normal cctv zones that arent conflict or pre conflict
         */
        CctvZone( unsigned long atsValue );


        /**
         * copy constructor
         *
         * @param rhs    The struct to compare to
         */
        CctvZone( const CctvZone& rhs );


        /**
         * Destructor
         */
        ~CctvZone();


        /**
         * assignment operator
         *
         * @param rhs
         */
        void operator=( const double& rhs );


        /**
         * == operator to check equality
         *
         * @param rhs    The struct to compare to
         */
        bool operator==( const CctvZone& rhs ) const;
        
        
        /**
         * =! operator to check not equality
         *
         * @param rhs    The struct to compare to
         */
        bool operator!=( const CctvZone& rhs ) const;


        /**
         * query operator to assign the double value of this object directly to a double
         */
        double operator()() const;


        /**
         * Gets the stage the train is currently in
         *
         * @return the stage the train is currently in
         */
        unsigned long getCurrentStage() const;


        /**
         * Gets the stage the train will soon be in
         *
         * @return the stage the train will soon be in
         */
        unsigned long getNextStage() const;


        /**
         * Gets the type of zone this is
         *
         * @return the type of zone this is
         */
        ECctvZoneType getZoneType() const;


    private:

        /**
         * This will convert the m_doubleRepresentation into the useable current, next, and zonetype represenation.
         */
        void convertFromDouble();


        /**
         * The raw double value from ATS
         */
        double m_doubleRepresentation;


        /**
         * The stage the train is currently in
         */
        unsigned long m_currentStage;


        /**
         * The stage the train will soon be in
         */
        unsigned long m_nextStage;


        /**
         * The type of zone this is
         */
        ECctvZoneType m_zoneType;

    };

} // TA_IRS_App

#endif // !defined(CctvZone_H)
