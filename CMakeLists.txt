cmake_minimum_required (VERSION 3.10.0)

project (mcp)

set (VERSION "1.1.10")

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib:\$ORIGIN/")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set (BOOST_CUSTOM OFF CACHE BOOL "")

set(MCP_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${MCP_CMAKE_DIR})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if (WIN32)
	set (PLATFORM_COMPILE_FLAGS "-DBOOST_SPIRIT_THREADSAFE -DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED -DETH_FATDB -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DWIN32_LEAN_AND_MEAN -DMINIUPNP_STATICLIB -DNOMINMAX")
else (WIN32)
	set (PLATFORM_COMPILE_FLAGS "-DBOOST_SPIRIT_THREADSAFE -DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED -DETH_FATDB -Werror=switch -fPIC")
	if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86(_64)?)$")
		if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			set (PLATFORM_COMPILE_FLAGS "${PLATFORM_COMPILE_FLAGS} -msse2")
		endif()
	endif()
endif (WIN32)

if (WIN32)
	set (PLATFORM_C_FLAGS "/std=c11")
else (WIN32)
	set (PLATFORM_C_FLAGS "-std=c11")
endif (WIN32)

if (WIN32)
	set (PLATFORM_CXX_FLAGS "-std:c++17 /bigobj /MP /wd4086 /wd4996 /wd4503 /wd4267 /wd4180 /wd4290 /wd4297 /wd4244 /wd4800")
else (WIN32)
	set (PLATFORM_CXX_FLAGS "-std=c++17 ")
endif (WIN32)

if (WIN32)
	set (PLATFORM_GUI_TARGET_PROPERTIES WIN32)
else (WIN32)
	set (PLATFORM_GUI_TARGET_PROPERTIES "")
endif (WIN32)

if (APPLE)
	set (PLATFORM_LINK_FLAGS "-framework Foundation -framework OpenCL")
elseif (WIN32)
	set (PLATFORM_LINK_FLAGS "")
else ()
	set (PLATFORM_LINK_FLAGS "-g -static-libgcc -static-libstdc++")
endif (APPLE)

include(ProjectLibFF)

include_directories (${CMAKE_SOURCE_DIR})

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)

if (BOOST_CUSTOM)
    list(APPEND CMAKE_MODULE_PATH ".\\" ${CMAKE_SOURCE_PATH})
endif (BOOST_CUSTOM)

find_package (Boost 1.81.0 REQUIRED COMPONENTS date_time filesystem system log log_setup thread program_options regex chrono atomic)
include_directories (${Boost_INCLUDE_DIR})

set(rocksdb_INCLUDE_DIR "" CACHE PATH "rocksdb include directory")
set(rocksdb_LIBRARY_DEBUG "" CACHE FILEPATH "Debug rocksdb library directory")
set(rocksdb_LIBRARY_RELEASE "" CACHE FILEPATH "Release rocksdb library directory")
if ((NOT rocksdb_INCLUDE_DIR STREQUAL "") OR (NOT rocksdb_LIBRARY_DEBUG STREQUAL "") OR (NOT rocksdb_LIBRARY_RELEASE STREQUAL ""))
	if (NOT EXISTS "${rocksdb_INCLUDE_DIR}/rocksdb/db.h")
	    message(FATAL_ERROR, "rocksdb_INCLUDE_DIR error")
		return()
	endif()
	if (NOT EXISTS ${rocksdb_LIBRARY_DEBUG})
	    message(FATAL_ERROR, "rocksdb_LIBRARY_DEBUG error")
		return()
	endif()
	if (NOT EXISTS ${rocksdb_LIBRARY_RELEASE})
	    message(FATAL_ERROR, "rocksdb_LIBRARY_RELEASE error")
		return()
	endif()
	
	include_directories (${rocksdb_INCLUDE_DIR})
	set(rocksdb_LIBRARY debug ${rocksdb_LIBRARY_DEBUG} optimized ${rocksdb_LIBRARY_RELEASE})
else ()
    set(rocksdb_LIBRARY "")
endif()


set(ZSTD_LIBRARY_DEBUG "" CACHE FILEPATH "Debug ZSTD library directory")
set(ZSTD_LIBRARY_RELEASE "" CACHE FILEPATH "Release ZSTD library directory")
if ((NOT ZSTD_LIBRARY_DEBUG STREQUAL "") OR (NOT ZSTD_LIBRARY_RELEASE STREQUAL ""))
	if (NOT EXISTS ${ZSTD_LIBRARY_DEBUG})
	    message(FATAL_ERROR, "ZSTD_LIBRARY_DEBUG error")
		return()
	endif()
	if (NOT EXISTS ${ZSTD_LIBRARY_RELEASE})
	    message(FATAL_ERROR, "ZSTD_LIBRARY_RELEASE error")
		return()
	endif()
	
	set(ZSTD_LIBRARY debug ${ZSTD_LIBRARY_DEBUG} optimized ${ZSTD_LIBRARY_RELEASE})
else ()
    set(ZSTD_LIBRARY "")
endif()

set(lz4_LIBRARY_DEBUG "" CACHE FILEPATH "Debug lz4 library directory")
set(lz4_LIBRARY_RELEASE "" CACHE FILEPATH "Release lz4 library directory")
if ((NOT lz4_LIBRARY_DEBUG STREQUAL "") OR (NOT lz4_LIBRARY_RELEASE STREQUAL ""))
	if (NOT EXISTS ${lz4_LIBRARY_DEBUG})
	    message(FATAL_ERROR, "lz4_LIBRARY_DEBUG error")
		return()
	endif()
	if (NOT EXISTS ${lz4_LIBRARY_RELEASE})
	    message(FATAL_ERROR, "lz4_LIBRARY_RELEASE error")
		return()
	endif()
	
    set(lz4_LIBRARY debug ${lz4_LIBRARY_DEBUG} optimized ${lz4_LIBRARY_RELEASE})
else ()
    set(lz4_LIBRARY "")
endif()

set(ZLIB_LIB_DEBUG "" CACHE FILEPATH "Debug ZLIB library directory")
set(ZLIB_LIB_RELEASE "" CACHE FILEPATH "Release ZLIB library directory")
if ((NOT ZLIB_LIB_DEBUG STREQUAL "") OR (NOT ZLIB_LIB_RELEASE STREQUAL ""))
	if (NOT EXISTS ${ZLIB_LIB_DEBUG})
	    message(FATAL_ERROR, "ZLIB_LIB_DEBUG error")
		return()
	endif()
	if (NOT EXISTS ${ZLIB_LIB_RELEASE})
	    message(FATAL_ERROR, "ZLIB_LIB_RELEASE error")
		return()
	endif()
	
    set(ZLIB_LIBRARY debug ${ZLIB_LIB_DEBUG} optimized ${ZLIB_LIB_RELEASE})
else ()
    set(ZLIB_LIBRARY "")
endif()

if (WIN32)	
	if ((rocksdb_INCLUDE_DIR STREQUAL "") OR (rocksdb_LIBRARY STREQUAL ""))
	    message(FATAL_ERROR, "rocksdb_INCLUDE_DIR or rocksdb_LIBRARY can not be null")
		return()
	endif()
	
	if (ZSTD_LIBRARY STREQUAL "")
	    message(FATAL_ERROR, "ZSTD_LIBRARY can not be null")
		return()
	endif()
	
	if (lz4_LIBRARY STREQUAL "")
	    message(FATAL_ERROR, "lz4_LIBRARY can not be null")
		return()
	endif()
	
	if (ZLIB_LIBRARY STREQUAL "")
	    message(FATAL_ERROR, "ZLIB_LIBRARY can not be null")
		return()
	endif()
endif()


add_library (db
	mcp/db/database.cpp
	mcp/db/database.hpp
	mcp/db/db_iterator.cpp
	mcp/db/db_iterator.hpp
	mcp/db/db_transaction.cpp
	mcp/db/db_transaction.hpp
	mcp/db/write_batch.cpp
	mcp/db/write_batch.hpp
	mcp/db/column.cpp
	mcp/db/column.hpp
	mcp/db/counter.cpp
	mcp/db/counter.hpp
	mcp/db/common.hpp
	mcp/db/common.cpp)

include_directories("${CMAKE_SOURCE_DIR}/mcp/p2p")
include_directories("${CMAKE_SOURCE_DIR}/miniupnp")
include_directories("${CMAKE_SOURCE_DIR}/lz4/lib")
include_directories("${CMAKE_SOURCE_DIR}/evmc/include")
include_directories("${CMAKE_SOURCE_DIR}/secp256k1/src")
include_directories("${CMAKE_SOURCE_DIR}/secp256k1/include")
include_directories("${CMAKE_SOURCE_DIR}/intx/include")

set (CRYPTOPP_LIBRARY cryptopp-static)
set (BUILD_SHARED OFF CACHE BOOL "")
set (BUILD_TESTING OFF CACHE BOOL "")
set (USE_INTERMEDIATE_OBJECTS_TARGET OFF CACHE BOOL "")
set (DISABLE_NATIVE_ARCH ON CACHE BOOL "")

add_subdirectory (secp256k1)
add_subdirectory (cryptopp)
add_subdirectory(evmc)
add_subdirectory(libevm)
add_subdirectory(libdevcore)
add_subdirectory(libdevcrypto)
add_subdirectory(account)
add_subdirectory(libinterpreter)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set (PLATFORM_SECURE_SOURCE mcp/plat/osx/working.mm)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set (PLATFORM_SECURE_SOURCE mcp/plat/windows/working.cpp)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set (PLATFORM_SECURE_SOURCE mcp/plat/posix/working.cpp)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	set (PLATFORM_SECURE_SOURCE mcp/plat/posix/working.cpp)
else ()
	error ("Unknown platform: ${CMAKE_SYSTEM_NAME}")
endif ()

add_library (common
	${PLATFORM_SECURE_SOURCE}
	mcp/common/utility.cpp
	mcp/common/utility.hpp
	mcp/common/alarm.cpp
	mcp/common/alarm.hpp
	mcp/common/async_task.hpp
	mcp/common/pwd.hpp
	mcp/common/pwd.cpp
	mcp/common/working.hpp
	mcp/common/numbers.cpp
	mcp/common/numbers.hpp
	mcp/common/base58.h
	mcp/common/base58.cpp
	mcp/common/stopwatch.hpp
	mcp/common/stopwatch.cpp
	mcp/common/lruc_cache.hpp
	mcp/common/log.cpp
	mcp/common/log.hpp
 	mcp/common/json.hpp
 	mcp/common/fifo_map.hpp    
	mcp/common/mcp_json.hpp
	mcp/common/assert.hpp
	mcp/common/common.hpp
	mcp/common/common.cpp
	)

	
FILE(GLOB NODE_EVM_SOURCES
	mcp/node/evm/Executive.hpp
	mcp/node/evm/Executive.cpp
	mcp/node/evm/ExtVM.h
	mcp/node/evm/ExtVM.cpp
)
SOURCE_GROUP(evm FILES ${NODE_EVM_SOURCES})

FILE(GLOB NODE_TRACE_SOURCES
	mcp/node/tracers/Tracer.hpp
	mcp/node/tracers/Tracer.cpp
	mcp/node/tracers/OpCode.hpp
	mcp/node/tracers/OpCode.cpp
	mcp/node/tracers/4byte.hpp
	mcp/node/tracers/4byte.cpp
	mcp/node/tracers/Call.hpp
	mcp/node/tracers/Call.cpp
	mcp/node/tracers/PreState.hpp
	mcp/node/tracers/PreState.cpp
)
SOURCE_GROUP(tracers FILES ${NODE_TRACE_SOURCES})

add_library (node
	${NODE_EVM_SOURCES}
	${NODE_TRACE_SOURCES}
	mcp/node/chain.hpp
	mcp/node/chain.cpp
	mcp/node/chain_state.hpp
	mcp/node/chain_state.cpp
	mcp/node/composer.hpp
	mcp/node/composer.cpp
	mcp/node/sync.hpp
	mcp/node/sync.cpp
	mcp/node/node_capability.hpp
	mcp/node/node_capability.cpp
	mcp/node/witness.hpp
	mcp/node/witness.cpp
	mcp/node/block_processor.cpp
	mcp/node/block_processor.hpp
	mcp/node/process_block_cache.hpp
	mcp/node/process_block_cache.cpp
	mcp/node/message.cpp
	mcp/node/message.hpp
	mcp/node/unhandle.cpp
	mcp/node/unhandle.hpp
	mcp/node/arrival.cpp
	mcp/node/arrival.hpp
	mcp/node/arrival.hpp
	mcp/node/debug.cpp
	mcp/node/debug.hpp
	mcp/node/transaction_queue.cpp
	mcp/node/transaction_queue.hpp
	mcp/node/common.cpp
	mcp/node/common.hpp
	mcp/node/approve_queue.cpp
	mcp/node/approve_queue.hpp
	mcp/node/requesting.cpp
	mcp/node/requesting.hpp
	mcp/node/Block.hpp
	mcp/node/Block.cpp
	mcp/node/Client.hpp
	mcp/node/Client.cpp
	mcp/node/VerifiedBlock.hpp
    )

add_library (p2p
	${PLATFORM_P2P_SOURCE}
	mcp/p2p/common.hpp
	mcp/p2p/common.cpp
	mcp/p2p/capability.hpp
	mcp/p2p/capability.cpp
	mcp/p2p/frame_coder.hpp
	mcp/p2p/frame_coder.cpp
	mcp/p2p/handshake.hpp
	mcp/p2p/handshake.cpp
	mcp/p2p/peer.hpp
	mcp/p2p/peer.cpp
	mcp/p2p/peer_manager.hpp
	mcp/p2p/peer_manager.cpp
	mcp/p2p/peer_store.hpp
	mcp/p2p/peer_store.cpp
	mcp/p2p/host.hpp
	mcp/p2p/host.cpp
	mcp/p2p/node_table.hpp
	mcp/p2p/node_table.cpp
	mcp/p2p/node_entry.hpp
	mcp/p2p/node_entry.cpp
	mcp/p2p/discover_packet.hpp
	mcp/p2p/discover_packet.cpp
	mcp/p2p/upnp.hpp
	mcp/p2p/upnp.cpp)
	
add_library (rpc
	mcp/rpc/rpc.cpp
	mcp/rpc/rpc.hpp
	mcp/rpc/config.cpp
	mcp/rpc/config.hpp
	mcp/rpc/connection.cpp
	mcp/rpc/connection.hpp
	mcp/rpc/handler.cpp
	mcp/rpc/handler.hpp
	mcp/rpc/rpc_ws.cpp
	mcp/rpc/rpc_ws.hpp
	mcp/rpc/jsonHelper.cpp
	mcp/rpc/jsonHelper.hpp
	mcp/rpc/json.cpp
	mcp/rpc/json.hpp
	mcp/rpc/exceptions.hpp
	)
	
add_library (wallet
	mcp/wallet/wallet.cpp
	mcp/wallet/wallet.hpp
	mcp/wallet/key_manager.cpp
	mcp/wallet/key_manager.hpp
	mcp/wallet/key_store.cpp
	mcp/wallet/key_store.hpp
	)
	
add_library (consensus
	mcp/consensus/ledger.cpp
	mcp/consensus/ledger.hpp
	mcp/consensus/validation.hpp
	mcp/consensus/validation.cpp
	)
	
add_library (core
	mcp/core/common.cpp
	mcp/core/common.hpp
	mcp/core/genesis.cpp
	mcp/core/genesis.hpp
	mcp/core/config.cpp
	mcp/core/config.hpp
	mcp/core/param.cpp
	mcp/core/param.hpp
	mcp/core/blocks.cpp
	mcp/core/blocks.hpp
	mcp/core/block_store.cpp
	mcp/core/block_store.hpp
	mcp/core/block_cache.cpp
	mcp/core/block_cache.hpp
	mcp/core/graph.cpp
	mcp/core/graph.hpp
	mcp/core/timeout_db_transaction.hpp
	mcp/core/timeout_db_transaction.cpp
	mcp/core/overlay_db.hpp
	mcp/core/overlay_db.cpp
	mcp/core/log_entry.hpp
	mcp/core/log_entry.cpp
	mcp/core/transaction_receipt.hpp
	mcp/core/transaction_receipt.cpp
	mcp/core/transaction.hpp
	mcp/core/transaction.cpp
	mcp/core/transaction_queue.hpp
	mcp/core/approve_receipt.hpp
	mcp/core/approve_receipt.cpp
	mcp/core/approve.hpp
	mcp/core/approve.cpp
	mcp/core/contract.hpp
	mcp/core/contract.cpp
	mcp/core/Precompiled.h
	mcp/core/Precompiled.cpp
	mcp/core/SealEngine.h
	mcp/core/SealEngine.cpp
	mcp/core/ChainOperationParams.hpp
	mcp/core/ChainOperationParams.cpp
	mcp/core/iapprove_queue.hpp
	mcp/core/LogFilter.cpp
	mcp/core/LogFilter.hpp
	)
	
add_executable (mcp
	mcp/application/daemon.cpp
	mcp/application/daemon.hpp
	mcp/application/entry.cpp
	mcp/application/cmdline.cpp
	mcp/application/cmdline.hpp
	)

add_executable (test_account
	test/account/main.hpp
	test/account/main.cpp
	test/account/account.cpp
	test/account/crypto.cpp
	test/account/abi.cpp
	test/account/vrf.cpp
	test/account/secure_string.cpp)

set (UPNPC_BUILD_SHARED OFF CACHE BOOL "")
set (UPNPC_BUILD_SAMPLE OFF CACHE BOOL "")
set (UPNPC_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(miniupnp/miniupnpc)

#lz4
add_subdirectory(lz4/contrib/cmake_unofficial)

set_target_properties (db PROPERTIES COMPILE_FLAGS "${PLATFORM_CXX_FLAGS} ${PLATFORM_COMPILE_FLAGS}")
set_target_properties (common rpc wallet consensus core node devcrypto devcore p2p mcp evm interpreter account PROPERTIES COMPILE_FLAGS "${PLATFORM_CXX_FLAGS} ${PLATFORM_COMPILE_FLAGS} -DQT_NO_KEYWORDS -DMCP_VERSION=${VERSION} -DBOOST_ASIO_HAS_STD_ARRAY=1")
set_target_properties (common rpc wallet consensus core node devcrypto devcore p2p mcp PROPERTIES LINK_FLAGS "${PLATFORM_LINK_FLAGS}")
set_target_properties (libminiupnpc-static PROPERTIES COMPILE_FLAGS "${PLATFORM_C_FLAGS} ${PLATFORM_COMPILE_FLAGS}")

set_target_properties (test_account PROPERTIES COMPILE_FLAGS "${PLATFORM_CXX_FLAGS} ${PLATFORM_COMPILE_FLAGS} -DQT_NO_KEYWORDS -DMCP_VERSION=${VERSION} -DBOOST_ASIO_HAS_STD_ARRAY=1")
set_target_properties (test_account PROPERTIES LINK_FLAGS "${PLATFORM_LINK_FLAGS}")

if (WIN32)
	set (PLATFORM_LIBS Ws2_32 mswsock iphlpapi ntdll Rpcrt4 Shlwapi)
elseif (APPLE)
	set (PLATFORM_LIBS pthread ${CMAKE_DL_LIBS})
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib")
else ()
    set (PLATFORM_LIBS pthread rt ${CMAKE_DL_LIBS})
	LINK_DIRECTORIES(/usr/local/lib/)
    LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu/)
endif (WIN32)

if (rocksdb_LIBRARY STREQUAL "")
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} rocksdb)
else ()
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} ${rocksdb_LIBRARY})
endif()

if (ZSTD_LIBRARY STREQUAL "")
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} zstd)
else ()
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} ${ZSTD_LIBRARY})
endif()

if (lz4_LIBRARY STREQUAL "")
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} lz4)
else ()
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} ${lz4_LIBRARY})
endif()

if (ZLIB_LIBRARY STREQUAL "")
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} z)
else ()
    set (DEPENDENCE_LIBS ${DEPENDENCE_LIBS} ${ZLIB_LIBRARY})
endif()

target_link_libraries (mcp rpc wallet consensus node p2p core db common account devcrypto devcore evm interpreter libminiupnpc-static secp256k1 lz4_static ${CRYPTOPP_LIBRARY} ${Boost_ATOMIC_LIBRARY} ${Boost_CHRONO_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${Boost_THREAD_LIBRARY} ${PLATFORM_LIBS}  ${DEPENDENCE_LIBS})

target_link_libraries (test_account rpc wallet consensus node p2p core db common account devcrypto devcore evm interpreter libminiupnpc-static secp256k1 lz4_static ${CRYPTOPP_LIBRARY} ${Boost_ATOMIC_LIBRARY} ${Boost_CHRONO_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${Boost_THREAD_LIBRARY} ${PLATFORM_LIBS} ${DEPENDENCE_LIBS})


