#ifndef MPGP_MPUTIL_GLOBAL_H
#define MPGP_MPUTIL_GLOBAL_H

/*#include <vld.h>*/
#include <QtCore/qglobal.h>

#ifdef DCUTIL_LIB
# define MPUTIL_EXPORT Q_DECL_EXPORT
#else
# define MPUTIL_EXPORT Q_DECL_IMPORT
#endif

#endif // MPGP_MPUTIL_GLOBAL_H
