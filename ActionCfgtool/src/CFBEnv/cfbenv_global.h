#pragma once

#include <QtCore/qglobal.h>

#ifndef QT_STATIC
# if defined(CFBENV_LIB)
#  define CFBENV_EXPORT Q_DECL_EXPORT
# else
#  define CFBENV_EXPORT Q_DECL_IMPORT
# endif
#else
# define CFBENV_EXPORT
#endif
