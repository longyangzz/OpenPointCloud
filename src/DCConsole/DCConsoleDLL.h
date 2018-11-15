#ifndef DC_CONSOLEDLL_H
#define DC_CONSOLEDLL_H

/*#include <vld.h>*/
#include "QtGlobal"

#ifdef DCCONSOLE_LIB
#define DC_CONSOLE_API Q_DECL_EXPORT
#else
#define DC_CONSOLE_API Q_DECL_IMPORT
#endif

#endif // DC_CONSOLE_DLL_H
