/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/scada_historian_data_access/src/InsertSavePtr.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// InsertSavePtr.h: interface for the InsertSavePtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVEPTR_H__AFA29D36_6EF4_4F78_9E29_188591C901F1__INCLUDED_)
#define AFX_SAVEPTR_H__AFA29D36_6EF4_4F78_9E29_188591C901F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <assert.h>


namespace TA_App
{
    
        // Need this reference counter class for our smart ptr template.
        // It becomes part of any deriving class and places a reference counter in it.
        class RefCount 
        {
        public:
           RefCount(void)                          { myRefs = 0; }
   
           virtual ~RefCount()           
           {
              if ( myRefs != 0 )
              {
                 // somebody managed to kill this object without going through the
                 // wrapper and now we have dangeling references to this dead object.
                 // While debugging stop it right here.
                 assert( myRefs == 0 );
              }
           }

           unsigned long rfInc(void)               { return ++myRefs; }
           unsigned long rfDec(void)               { return --myRefs; }

        private:
           unsigned long myRefs;
        };


        //-----------------------------------------------------------------------------
        // Example how to use it:
        // The class handled as a smart ptr has to be derived from the RefCount class.
        //
        // class Sample : public RefCount 
        // {
        // public:
        //     void doSomething(void) {}
        // };


        //-----------------------------------------------------------------------------
        // The template for the smart ptr
        //
        //

        template <class T> class Ptr 
        {
        public:
           Ptr() : p(0)               {}
           Ptr(T* pt) : p(pt)         { p->rfInc(); }
           Ptr(Ptr<T> & pt) : p(pt)   { p->rfInc(); }
           Ptr(T* & pt) : p(pt)       { p->rfInc(); }

           virtual ~Ptr(void)
           { 
              if ( p && p->rfDec() == 0 )
              {
                 delete p;
                 p = 0;
              }
           }

           void  destroy()                  { delete this; }

           operator T*(void)                { return p; }
           T& operator*(void)               { return *p; }
           T* operator->(void)              { return p; }

           Ptr& operator=(Ptr<T> &pt)       { return operator=((T *) pt); }
           Ptr& operator=(T* pt) 
           {
              // We currently look to an object via 'p'. We don't want to look
              // at this particular object anymore. If we are the last one to
              // look at it then it's our responsibility to delete it. Otherwise
              // we leave the pointer 'p' alone because other entities may look at it.
              // In this case we simply forget the pointer 'p' and replace it.
              // We can overwrite our ptr because the last entity who looks at
              // the object pointed to by 'p' will delete it.
              if ( p && p->rfDec() == 0 ) 
              {
                 delete p;
                 p = 0;
              }

              // Set our 'view' to the new object pointed by pt.
              p = pt; 
      
              // Tell object pointed to by 'p' that one more entity (us) is looking at.
              if ( p )
              {
                 p->rfInc(); 
              }

              return *this;
           }

        private:
           // The current object we look at.
           T* p;
        };

    
} // TA_App


#endif // !defined(AFX_SAVEPTR_H__AFA29D36_6EF4_4F78_9E29_188591C901F1__INCLUDED_)

