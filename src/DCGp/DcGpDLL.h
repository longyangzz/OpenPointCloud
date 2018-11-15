#ifndef DCGP_GLOBAL_H
#define DCGP_GLOBAL_H

/*#include <vld.h>*/
#include "QtCore/qglobal.h"

#ifdef DCGP_LIB
# define DCGP_API Q_DECL_EXPORT
#else
# define DCGP_API Q_DECL_IMPORT
#endif

#endif // DCGP_GLOBAL_H
