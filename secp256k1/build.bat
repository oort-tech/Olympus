
#call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"

cl -I. -DECMULT_GEN_PREC_BITS=4 src/gen_context.c
gen_context.exe

cl /c /MT -I. -DENABLE_MODULE_RECOVERY=1 -DENABLE_MODULE_ECDH=1 -DUSE_NUM_NONE=1 -DUSE_ECMULT_STATIC_PRECOMPUTATION=1 -DECMULT_GEN_PREC_BITS=4 -DUSE_FIELD_INV_BUILTIN=1 -DUSE_SCALAR_INV_BUILTIN=1 -DUSE_FIELD_10X26=1 -DUSE_SCALAR_8X32=1 -DECMULT_WINDOW_SIZE=15 src/secp256k1.c
lib secp256k1.obj
dir
if not exist static mkdir static
move /y secp256k1.lib static\secp256k1-vrf.lib

cl /MT /D_USRDLL /D_WINDLL -I. -DENABLE_MODULE_RECOVERY=1 -DENABLE_MODULE_ECDH=1 -DSECP256K1_BUILD -DUSE_NUM_NONE=1 -DUSE_ECMULT_STATIC_PRECOMPUTATION=1 -DECMULT_GEN_PREC_BITS=4 -DUSE_FIELD_INV_BUILTIN=1 -DUSE_SCALAR_INV_BUILTIN=1 -DUSE_FIELD_10X26=1 -DUSE_SCALAR_8X32=1 -DECMULT_WINDOW_SIZE=15 src/secp256k1.c src\win32\dllmain.c /LD /Fesecp256k1-vrf.dll /link
dir
dumpbin /dependents secp256k1-vrf.dll

mkdir x64
move static x64\
move secp256k1-vrf.dll x64\
move secp256k1-vrf.lib x64\
