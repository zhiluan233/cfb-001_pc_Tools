#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CFBUIBASE_LIB)
#  define CFBUIBASE_EXPORT Q_DECL_EXPORT
# else
#  define CFBUIBASE_EXPORT Q_DECL_IMPORT
# endif
#else
# define CFBUIBASE_EXPORT
#endif
