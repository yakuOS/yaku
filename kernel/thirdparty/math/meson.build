kernel_third_party_math_c_srcs = files([
    'abs.c',
    'scalbnf.c',
    'atan2.c',
    'fabsl.c',
    'ldexpl.c',
    'nexttowardl.c',
    '__expo2.c',
    'sincos.c',
    'tanh.c',
    'sincosf.c',
    'frexpl.c',
    'sin.c','j0f.c',
    'remquof.c',
    'atanl.c',
    'log2f.c',
    'llrintf.c',
    'powf.c',
    'atan2l.c',
    'llrint.c',
    'ldexp.c',
    '__signbitf.c',
    'fabsf.c',
    'fminl.c',
    'jnf.c',
    'llround.c',
    'log1pf.c',
    '__signbitl.c',
    'fdimf.c',
    'ilogb.c',
    'tan.c',
    '__rem_pio2.c',
    'nexttowardf.c',
    'fmodl.c','log.c',
    'tgammaf.c',
    'atanf.c',
    '__tandf.c',
    '__fpclassify.c',
    'fmaxf.c',
    'log10f.c',
    'sincosl.c',
    '__rem_pio2l.c',
    '__rem_pio2_large.c',
    'expm1l.c',
    'atanhl.c',
    '__cosl.c',
    'floor.c',
    'floorf.c',
    'sinf.c',
    'asinh.c',
    'fmin.c',
    'rint.c',
    'fma.c',
    'fmax.c',
    '__cos.c',
    'acoshf.c',
    'cosh.c',
    'logbl.c',
    'nexttoward.c',
    '__fpclassifyl.c',
    'trunc.c',
    'coshl.c',
    'remainder.c',
    'tanl.c',
    'modff.c',
    '__invtrigl.c',
    'atan2f.c',
    'exp10l.c',
    'scalblnf.c',
    'ldexpf.c',
    'logf.c',
    'log1pl.c',
    'sinl.c',
    'nan.c',
    'log1p.c',
    '__sindf.c',
    'fmodf.c',
    'expf.c',
    'expm1.c',
    '__rem_pio2f.c',
    '__tanl.c',
    'nanf.c',
    'logb.c',
    'fabs.c',
    'nextafterf.c',
    'jn.c',
    'expm1f.c',
    'remquo.c',
    '__signbit.c',
    'j1f.c',
    'fdim.c',
    'j1.c',
    'powl.c',
    'sqrt.c',
    'rintf.c',
    'scalbn.c',
    'sqrtl.c',
    'ceill.c',
    'frexpf.c',
    'tanhl.c',
    'scalbf.c',
    'asinl.c',
    'tgamma.c',
    'lroundf.c',
    'modf.c',
    'lroundl.c',
    'fdiml.c',
    'signgam.c',
    'nextafter.c',
    'significandf.c',
    'exp2l.c','cbrtf.c',
    'pow.c',
    'cbrtl.c',
    'lround.c',
    '__polevll.c',
    'lgammal.c',
    'rintl.c',
    'floorl.c',
    'ilogbf.c',
    'erff.c',
    'logbf.c',
    'ceil.c',
    'roundf.c',
    'llroundl.c',
    'cos.c',
    'lgammaf.c',
    'log10.c',
    'nextafterl.c',
    'exp.c',
    'ilogbl.c',
    'tgammal.c',
    'lgamma.c',
    '__cosdf.c',
    'scalbln.c',
    'lrintf.c',
    'acos.c',
    'lgamma_r.c',
    '__sin.c',
    'erf.c',
    'log2.c',
    'fmaxl.c',
    'sinhf.c',
    'scalblnl.c',
    'acoshl.c',
    'hypot.c',
    'lrint.c',
    'sinhl.c',
    'cosl.c',
    'exp10f.c',
    '__tan.c',
    'exp2f.c',
    'fmod.c',
    'finite.c',
    'acosh.c',
    'log10l.c',
    'llroundf.c',
    'remainderl.c',
    'nanl.c',
    'sinh.c',
    'coshf.c',
    'copysignl.c',
    'scalb.c',
    'asinhf.c',
    'expl.c',
    'log2l.c',
    'lgammaf_r.c',
    'atan.c',
    'acosf.c',
    '__expo2f.c',
    'remainderf.c',
    'cbrt.c',
    'remquol.c',
    'finitef.c',
    'atanhf.c',
    '__sinl.c',
    'logl.c',
    'modfl.c',
    'significand.c',
    'asinf.c',
    'asinhl.c',
    'hypotl.c',
    'copysign.c',
    'truncl.c',
    'ceilf.c',
    'truncf.c',
    'tanhf.c',
    'round.c',
    'cosf.c',
    'j0.c',
    'atanh.c',
    'asin.c',
    'hypotf.c',
    'scalbnl.c',
    'exp10.c',
    'exp2.c',
    'erfl.c',
    'copysignf.c',
    'fminf.c',
    'frexp.c',
    'tanf.c',
    'sqrtf.c',
    'roundl.c',
    '__fpclassifyf.c',
    'acosl.c'
])

kernel_third_party_math_objs = cc_gen.process(
    kernel_third_party_math_c_srcs,
    extra_args: kernel_ccflags
)

kernel_third_party_math_include_dirs = [meson.current_source_dir()]
