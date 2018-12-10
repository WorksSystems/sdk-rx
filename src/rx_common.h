#ifndef __RX_COMMON_H_
#define __RX_COMMON_H_

#ifndef DBG
#include <time.h>
#define DBG(x...) { char _ST_[22] = ""; time_t tt; time(&tt); strftime( _ST_, sizeof( _ST_ ), "%Y-%m-%dT%H:%M:%SZ", gmtime(&tt)); fprintf(stderr, "%s %16s:%04d |%12s(): ", _ST_, __FILE__, __LINE__, __FUNCTION__); }fprintf(stderr, x); fprintf(stderr, "\n")
#endif

#endif//__RX_COMMON_H_
