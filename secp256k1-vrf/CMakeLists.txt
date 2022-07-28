# This CMake config file for secp256k1 project from https://github.com/bitcoin-core/secp256k1
#
# The secp256k1 project has been configured following official docs with following options:
#
# ./configure --disable-shared --disable-tests --disable-coverage --disable-openssl-tests --disable-exhaustive-tests --disable-jni --with-bignum=no --with-field=64bit --with-scalar=64bit --with-asm=no
#
# Build static context:
# make src/ecmult_static_context.h
#
# Copy src/ecmult_static_context.h and src/libsecp256k1-config.h
#
# Copy CFLAGS from Makefile to COMPILE_OPTIONS.

set(COMMON_COMPILE_FLAGS ENABLE_MODULE_RECOVERY ENABLE_MODULE_ECDH USE_ECMULT_STATIC_PRECOMPUTATION USE_FIELD_INV_BUILTIN USE_NUM_NONE USE_SCALAR_INV_BUILTIN)
if (MSVC)
	set(COMPILE_FLAGS USE_FIELD_10X26 USE_SCALAR_8X32)
	set(COMPILE_OPTIONS "")
else()
	set(COMPILE_FLAGS USE_FIELD_5X52 USE_SCALAR_4X64 HAVE_BUILTIN_EXPECT HAVE___INT128)
	set(COMPILE_OPTIONS -O3 -W -std=c89 -pedantic -Wall -Wextra -Wcast-align -Wnested-externs -Wshadow -Wstrict-prototypes -Wno-unused-function -Wno-long-long -Wno-overlength-strings -fvisibility=hidden)
endif()

cmake_minimum_required(VERSION 3.5)

if (MSVC)
execute_process(
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND cmd /C "build.bat"
	OUTPUT_VARIABLE _stdout
	ERROR_VARIABLE _stderr
	OUTPUT_STRIP_TRAILING_WHITESPACE
	ERROR_STRIP_TRAILING_WHITESPACE
)
else()
include(ExternalProject)

ExternalProject_Add( static_secp256k1-vrf
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/configure --enable-module-recovery --enable-module-ecdh --enable-experimental --prefix=${CMAKE_CURRENT_SOURCE_DIR}/build
	BUILD_COMMAND make
)


ExternalProject_Add_Step(static_secp256k1-vrf autogen
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/ecmult_const_impl.h ${CMAKE_CURRENT_SOURCE_DIR}/
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/autogen.sh
	DEPENDERS configure
)

add_library(secp256k1-vrf STATIC IMPORTED)
set_property(TARGET secp256k1-vrf PROPERTY IMPORTED_LOCATION libsecp256k1-vrf.a)
set_property(TARGET secp256k1-vrf PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_dependencies(secp256k1-vrf static_secp256k1-vrf)
endif()


