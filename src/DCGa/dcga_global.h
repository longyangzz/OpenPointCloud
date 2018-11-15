#ifndef DCGA_GLOBAL_H
#define DCGA_GLOBAL_H

/*#include <vld.h>*/
#include <QtCore/qglobal.h>

#ifdef DCGA_LIB
# define DCGA_EXPORT Q_DECL_EXPORT
#else
# define DCGA_EXPORT Q_DECL_IMPORT
#endif

#endif // DCGA_GLOBAL_H
