/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/common/src/Element.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Element
  * Simple wrapper to contain update elements, with form of reference 
  * counting through the set object (in constructor)
  * 
  * There may be redundant data left over when hWnds are erased from
  * this class (leaving orphan data - which will be destroyed in destruction)
  * Not a problem, but possible efficiency improvement (only occurs when
  * splitAndCloneDataForWindow is called). 
  * Since this object typically has a short lifetime (used by UpdateBuffer), 
  * shouldn't be a problem (currently)
  * 
  */

#if !defined(Element_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define Element_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

#include <vector>
#include <set>
#include <map>

namespace TA_IRS_Bus
{

    template <class ELEM_TYPE> 
    class Element
    {
    public:
        /**
         * Element (constructor)
         *
         * @param hWnds the set of hWnds that are used to subsequently verify incoming fetch requests
         *
         * @param data the data to store in this container
         *
         */
        Element(const ELEM_TYPE& data, const std::set<HWND>& hWnds);
    
        virtual ~Element() {}
    
        /**
         * fetch
         * 
         * Returns the data set for this container, if appropriate
         * 
         * @pre getRemainingReferences() > 0
         * 
         * @param hWnd the identifier of the window that is requesting the data, must be recognized
         *         
         * @param bPeek set to true if want to peek at the packet, but not purge map
         *          of the message returned
         *
         * @exception ValueNotSetException if there is no data set for the specified hWnd
         *
         * @return data set for this container
         *
         */
        ELEM_TYPE fetch(HWND hWnd, bool bPeek);
    
        /**
         * getRemainingReferences
         * 
         * Returns the number of hWnds attached to this data still, if none, can discard this object
         *
         */
        DWORD getRemainingReferences() { return m_hWnds.size(); }
    
        /**
         * obtainMutableReference
         *
         * Returns a reference to the data relating exclusively to hWnd
         *  It is the full responsibility of the client code to ensure the necessary
         *  locks are in place to synchronise fetch and obtainMutableReference calls
         *
         * @param hWnd the window handle we want to obtain the data reference for
         *
         * @return reference to the modifiable data - this will not affect the data
         *          obtained by calls to fetchUpdate for any other hWnds (this is
         *          done by cloning and splitting the data apart, using copy constructor)
         *  
         * @exception ValueNotSetException if input hWnd unrecognized
         *
         */
        ELEM_TYPE&  obtainMutableReference(HWND hWnd);
    
    private:
    
        /**
         * getDataRef
         *
         * @return the current reference to data for input hWnd - there
         *      may be other windows pointing to same data (@see isSharingData)
         *
         * @param hWnd the window to obtain the data reference for
         *
         * @exception ValueNotSetException if window unrecognized
         *
         */
        ELEM_TYPE&  getDataRef(HWND hWnd);
    
         /**
         * getDataRef
         *
         * @return true if the data for hWnd is being referenced by multiple
         *          windows
         *
         * @param hWnd the window to obtain the data reference for
         *
         * @exception ValueNotSetException if window unrecognized
         *
         */
        bool  isSharingData(HWND hWnd);
    
        /**
         * splitAndCloneForWindow
         *
         * For windows sharing data, this function makes a copy of the data
         *  and provides both windows with independent copies, so that
         *  getDataRef returns a unique reference between this window, and
         *  any other window)
         *
         * @return the reference to the newly cloned data, exclusive to this window
         *
         * @pre isSharingData(hWnd) returns true (otherwise no point in calling function)
         *
         * @exception ValueNotSetException if window unrecognized
         *
         */
        ELEM_TYPE& splitAndCloneDataForWindow(HWND hWnd);
    
        // Actual data - usually only of size one
        std::vector<ELEM_TYPE> m_data;
    
        // Maps the window handles to the actual data (typically a many to one relationship)
        // But if obtainMutableReference is called, it becomes many to many
        std::map<HWND, unsigned long>   m_map;
    
        // Set of hWnd's associated with this data
        std::set<HWND> m_hWnds;    
    };

} // end namespace TA_IRS_Bus

// Template class required inclusion of inline definition
#include "bus/pa/common/src/Element.inl"

#endif // !defined(Element_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
