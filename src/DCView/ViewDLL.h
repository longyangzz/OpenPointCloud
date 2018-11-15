#ifndef DC_VIEW_DLL_H
#define DC_VIEW_DLL_H

/*#include <vld.h>*/
#include "QtCore/QtGlobal"

#ifdef DCVIEW_LIB
#define DC_VIEW_API Q_DECL_EXPORT
#else
#define DC_VIEW_API Q_DECL_IMPORT
#endif

#endif // VIEWER_DLL_H
