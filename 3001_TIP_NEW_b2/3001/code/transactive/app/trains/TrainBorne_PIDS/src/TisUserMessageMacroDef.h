#ifndef TISUSERMESSAGEMACRODEF_H
#define TISUSERMESSAGEMACRODEF_H

#define WM_UPDATE_CURRENT_TTIS_VERSION  (WM_USER + 110)
// for the next version of the pre-defined message library
#define WM_UPDATE_NEXT_TTIS_VERSION     (WM_USER + 111)
// for library synchronisation updates
#define WM_UPDATE_LIBRARY_SYNCHRONISED  (WM_USER + 112)
// for time schedule updates
#define WM_UPDATE_TIMESCHEDULE          (WM_USER + 113)
// for time schedule updates
#define WM_UPDATE_CURRENT_TIME_SCHEDULE (WM_USER + 114)

// for train download updates
#define WM_UPDATE_DOWNLOADLIST          (WM_USER + 115)
// for train data version updates
#define WM_UPDATE_TRAINVERSION          (WM_USER + 116)
// for rights
#define WM_UPDATE_RIGHTS                (WM_USER + 117)
// for filter change
#define WM_FILTER_CHANGE				(WM_USER + 118)

#define WM_UPDATEINBOUND                (WM_USER + 119)
#define WM_UPDATEOUTBOUND               (WM_USER + 120)
#define WM_UPDATESELECTIONCOMBO         (WM_USER + 121)

#endif
