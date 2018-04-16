#if !defined(AFX_EXPORTFILEDIALOG_H__7C84097E_3EF4_4A53_898D_DB67DC5BDD83__INCLUDED_)
#define AFX_EXPORTFILEDIALOG_H__7C84097E_3EF4_4A53_898D_DB67DC5BDD83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ExportFileDialog.h $
 * @author:  Bart Golab
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Report export file save dialog. Forces the operator to save the file into 
 * the configured export directory.
 *
 */

#include "bus\mfc_extensions\src\standard_dialogs_arial_bold\FileDialogArial.h"

namespace TA_Base_App
{
    class CExportFileDialog : public CFileDialogArial
    {
	    DECLARE_DYNAMIC(CExportFileDialog)

    public:
	    CExportFileDialog();
        virtual ~CExportFileDialog();

        CString getExportDirectory() const;

    private:
        CExportFileDialog(const CExportFileDialog&);
        CExportFileDialog& operator=(const CExportFileDialog&);

    protected:
	    //{{AFX_MSG(CExportFileDialog)
        virtual BOOL OnFileNameOK();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        CString m_exportDirectory;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXPORTFILEDIALOG_H__7C84097E_3EF4_4A53_898D_DB67DC5BDD83__INCLUDED_)
