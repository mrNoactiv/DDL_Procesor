#ifndef __log256table_h__
#define __log256table_h__

namespace common {
	namespace compression {
static const char LogTable256_1[256] = 
{
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    LT(5), LT(6), LT(6), LT(7), LT(7), LT(7), LT(7),
    LT(8), LT(8), LT(8), LT(8), LT(8), LT(8), LT(8), LT(8)
};

static const char LogTable256_0[256] = 
{
#define LT0(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    LT0(5), LT0(6), LT0(6), LT0(7), LT0(7), LT0(7), LT0(7),
    LT0(8), LT0(8), LT0(8), LT0(8), LT0(8), LT0(8), LT0(8), LT0(8)
};
	
	}}
#endif
