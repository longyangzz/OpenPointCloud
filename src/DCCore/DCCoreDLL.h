#ifndef DCCORE_GLOBAL_H
#define DCCORE_GLOBAL_H

/*#include <vld.h>*/
#include <QtCore/qglobal.h>

#ifdef DCCORE_LIB
# define DCCORE_EXPORT Q_DECL_EXPORT
#else
# define DCCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // DCCORE_GLOBAL_H
