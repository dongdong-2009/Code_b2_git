/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimFlexiMask.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#include <iostream>

namespace TA_IRS_App
{

namespace ATS_Sim
{

/* First some utility functions, required for the template */

/// utility template function to print hex value of any basic type such as
/// Byte Word long ulong etcetera
template <class T>
std::string mask2hexstr(T val)
{
    char formatstr[16] = {0};
    sprintf(formatstr,"0x%%0.%dx", sizeof(T)*2);

    char s[64] = {0};
    sprintf(s, formatstr, val);
    std::string ss = s;

    return ss;
}
 
/// utility template function to set one bit in a Byte Word long ulong etcetera
template <class T>
T bitSet(T val, int bitIndex)
{
    if (bitIndex > (sizeof(T)*8))
    {
        return val;
    }

    return (val | (0x0001 << bitIndex));
}

/// utility template function to clear one bit in a Byte Word long ulong etcetera
template <class T>
T bitClear(T val, int bitIndex)
{
    if (bitIndex > (sizeof(T)*8))
    {
        return val;
    }

    // bitwise-and with the bitwise-inversion of the bitIndex left-shifted
    return (val & (~(0x0001 << bitIndex)));
}

/// utility template function to test one bit in a Byte Word long ulong etcetera
template <class T>
bool bitTest(T val, int bitIndex)
{
    if (bitIndex > (sizeof(T)*8))
    {
        return false;
    }

    return ((val & (0x0001 << bitIndex)) != 0);
}


/** 
  */
template <class T>
class FlexiMask
{
    private:

    T m_toggleMask;
    T m_leaveMask;
    T m_valueMask;

    bool m_toggleSet;
    bool m_leaveSet;
    bool m_valueSet;

    public:
    FlexiMask()
    : m_toggleMask(0)
    , m_leaveMask(0)
    , m_valueMask(0)
    , m_toggleSet(false)
    , m_leaveSet(false)
    , m_valueSet(false)
    {
    }

    ~FlexiMask()
    {
    }

    void setValueBit(int bitIndex)
    {
        m_valueMask = bitSet( m_valueMask, bitIndex );
        // if the code using this function is setting bit-by-bit, it almost
        // certainly wants the corresponding leave and toggle bits cleared
        m_leaveMask = bitClear( m_leaveMask, bitIndex );
        m_toggleMask = bitClear( m_toggleMask, bitIndex );
        m_valueSet  = true;
    }

    void clearValueBit(int bitIndex)
    {
        m_valueMask = bitClear( m_valueMask, bitIndex );
        // if the code using this function is setting bit-by-bit, it almost
        // certainly wants the corresponding leave and toggle bits cleared
        m_leaveMask = bitClear( m_leaveMask, bitIndex );
        m_toggleMask = bitClear( m_toggleMask, bitIndex );
        m_valueSet  = true;
    }

    void setToggleBit(int bitIndex)
    {
        m_toggleMask = bitSet( m_toggleMask, bitIndex );
        // if the code using this function is setting bit-by-bit, it almost
        // certainly wants the corresponding leave and value bits cleared
        m_leaveMask = bitClear( m_leaveMask, bitIndex );
        m_valueMask = bitClear( m_valueMask, bitIndex );
        m_toggleSet  = true;
    }

    void setLeaveBit(int bitIndex)
    {
        m_leaveMask = bitSet( m_leaveMask, bitIndex );
        // if the code using this function is setting bit-by-bit, it almost
        // certainly wants the corresponding value and toggle bits cleared
        m_toggleMask = bitClear( m_toggleMask, bitIndex );
        m_valueMask = bitClear( m_valueMask, bitIndex );
        m_leaveSet  = true;
    }

    void setToggleMask(T toggle)
    {
        m_toggleMask = toggle;
        m_toggleSet  = true;
    }

    void setLeaveMask(T leave)
    {
        m_leaveMask = leave;
        m_leaveSet  = true;
    }

    void setValueMask(T value)
    {
        m_valueMask = value;
        m_valueSet  = true;
    }

    T getToggleMask() const
    {
        return m_toggleMask;
    }

    T getLeaveMask() const
    {
        return m_leaveMask;
    }

    T getValueMask() const
    {
        return m_valueMask;
    }

    T apply(T original)
    {
        T processed = original;
        T mask = 1;
        int oursize = sizeof(T) * 8;

        bool toggl_bit = false;
        bool leave_bit = false;
        bool set_bit   = false;
        
        for (int foo=0; foo<oursize ; foo++,mask<<=1)
        {
            toggl_bit = ( m_toggleSet && (( m_toggleMask & mask ) > 0));
            leave_bit = ( m_leaveSet && (( m_leaveMask & mask ) > 0));
            set_bit   = ( m_valueSet && (( m_valueMask & mask ) > 0));

            if ( ! leave_bit )
            {
                if ( toggl_bit )
                {
                    if ( original & mask )
                    {
                        // The bit is set. Clear it.
                        // bitwise-AND with the bitwise-NOT of the mask to clear the bit
                        processed &= (~mask);
                    }
                    else
                    {
                        // The bit is clear. Set it.
                        // bitwise-OR to set the bit
                        processed |= mask;
                    }
                }
                else if ( m_valueSet ) // set or clear accordingly
                {
                    if ( set_bit )
                    {
                        processed |= mask;
                    }
                    else // clear_bit
                    {
                        processed &= (~mask);
                    }
                }
                else
                {
                }
            }
            else
            {
                // 
                // leave the blessed bit alone.
            }

        }

        return processed;
    }

    bool setFromTL01String(const std::string & s)
    {
        const int oursize = sizeof(T) * 8;

        //
        // validate the string length
        if (s.length() != oursize)
        {
            return false;
        }

        //
        // validate the string contents
        for (int foo=0; foo<oursize ; foo++)
        {
            if (   (s[foo]!='1')
                && (s[foo]!='0')
                && (s[foo]!='T')
                && (s[foo]!='L') )
            {
                return false;
            }
        }

        //
        // Mostly confident now that this is going to work
        int maxidx=oursize-1;
        for (int foo2=0; foo2<oursize ; foo2++)
        {   // Don't really need a default clause, however it
            // sure shuts whinging, warning compilers up.
            switch (s[foo2])
            {
                case '1':
                    setValueBit(maxidx-foo2);
                    break;
                case '0':
                    clearValueBit(maxidx-foo2);
                    break;
                case 'T':
                    setToggleBit(maxidx-foo2);
                    break;
                case 'L':
                default:
                    setLeaveBit(maxidx-foo2);
                    break;
            }
        }

        return true;
    }

    bool isToggleUsed() const
    {
        return m_toggleSet;
    }

    bool isLeaveUsed() const
    {
        return m_leaveSet;
    }

    bool isValueUsed() const
    {
        return m_valueSet;
    }

    bool isUsed() const
    {
        // if the only mask set is the leave mask, we're not interested
        return (m_toggleSet || m_valueSet);
    }

    void reset()
    {
        m_toggleMask = 0;
        m_leaveMask = 0;
        m_valueMask = 0;
        m_toggleSet = false;
        m_leaveSet = false;
        m_valueSet = false;
    }

    std::ostream & operator<<(std::ostream & os)
    {
        if (m_toggleSet)
        {
            os << "toggle:" << mask2hexstr(m_toggleMask) << " ";
        }
        if (m_valueSet)
        {
            os << "value:" << mask2hexstr(m_valueMask) << " ";
        }
        if (m_leaveSet)
        {
            os << "leave:" << mask2hexstr(m_leaveMask) << " ";
        }

        return os;
    }

    friend std::ostream & operator<<(std::ostream & os, const FlexiMask<unsigned short> & mask);
    friend std::ostream & operator<<(std::ostream & os, const FlexiMask<unsigned char> & mask);
}; // class FlexiMask

inline std::ostream & operator<<(std::ostream & os, const FlexiMask<unsigned short> & mask)
{
    if (mask.m_toggleSet)
    {
        os << "toggle:" << mask2hexstr(mask.m_toggleMask) << " ";
    }
    if (mask.m_valueSet)
    {
        os << "value:" << mask2hexstr(mask.m_valueMask) << " ";
    }
    if (mask.m_leaveSet)
    {
        os << "leave:" << mask2hexstr(mask.m_leaveMask) << " ";
    }

    return os;
}

inline std::ostream & operator<<(std::ostream & os, const FlexiMask<unsigned char> & mask)
{
    if (mask.m_toggleSet)
    {
        os << "toggle:" << mask2hexstr(mask.m_toggleMask) << " ";
    }
    if (mask.m_valueSet)
    {
        os << "value:" << mask2hexstr(mask.m_valueMask) << " ";
    }
    if (mask.m_leaveSet)
    {
        os << "leave:" << mask2hexstr(mask.m_leaveMask) << " ";
    }

    return os;
}


} // namespace ATS_Sim
} // namespace TA_IRS_App
