#ifndef DSFMT_PARAMS_H_
#define DSFMT_PARAMS_H_

#include "dSFMT.h"

#define DSFMT_LOW_MASK    UINT64_C(0x000FFFFFFFFFFFFF)
#define DSFMT_HIGH_CONST  UINT64_C(0x3FF0000000000000)
#define DSFMT_SR          12

#if defined(__SSE2__)
#	define SSE2_SHUFF 0x1b
#elif defined(__ALTIVEC__)
#	if defined(__APPLE__)  /* For OSX */
#		define ALTI_SR (vector unsigned char)(4)
#		define ALTI_SR_PERM \
			(vector unsigned char)(15,0,1,2,3,4,5,6,15,8,9,10,11,12,13,14)
#		define ALTI_SR_MSK \
			(vector unsigned int)(0x000fffffU,0xffffffffU,0x000fffffU,0xffffffffU)
#		define ALTI_PERM \
			(vector unsigned char)(12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3)
#	else
#		define ALTI_SR      {4}
#		define ALTI_SR_PERM {15,0,1,2,3,4,5,6,15,8,9,10,11,12,13,14}
#		define ALTI_SR_MSK  {0x000fffffU,0xffffffffU,0x000fffffU,0xffffffffU}
#		define ALTI_PERM    {12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3}
#	endif
#endif

#include "dSFMT-params19937.h"

#endif
