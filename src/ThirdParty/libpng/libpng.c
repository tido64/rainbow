#define PNG_INTEL_SSE 1

#include <png.c>
#include <pngerror.c>
#include <pngget.c>
#include <pngmem.c>
#include <pngpread.c>
#include <pngread.c>
#include <pngrio.c>
#include <pngrtran.c>
#include <pngrutil.c>
#include <pngset.c>
#include <pngtrans.c>

#if PNG_ARM_NEON_OPT > 0
#    include <arm/arm_init.c>
#    include <arm/filter_neon_intrinsics.c>
#elif PNG_INTEL_SSE_OPT > 0
#    include <intel/filter_sse2_intrinsics.c>
#    include <intel/intel_init.c>
#elif PNG_MIPS_MSA_OPT > 0
#    include <mips/filter_msa_intrinsics.c>
#    include <mips/mips_init.c>
#elif PNG_POWERPC_VSX_OPT > 0
#    include <powerpc/filter_vsx_intrinsics.c>
#    include <powerpc/powerpc_init.c>
#endif
