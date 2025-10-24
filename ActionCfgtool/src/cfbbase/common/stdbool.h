

#ifndef _STDBOOL_H
#define _STDBOOL_H






#ifdef __BEOS__
# undef false
# undef true
#endif


#ifndef __cplusplus
# if !0
typedef enum { false = 0, true = 1 } _bool;
# endif
#else
typedef bool _bool;
#endif
#define bool _bool


#define false 0
#define true 1
#define __bool_true_false_are_defined 1

#endif 