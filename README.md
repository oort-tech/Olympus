## Olympus (The blockchain layer in Computecoin)
Official C++ implementation of the Olympus protocol.

To understand the consensus algorithm in Olympus, refer to the CCN yellow paper.\
https://resources.computecoin.com/docs/computecoin-consensus-and-security.pdf

[![API Reference](
https://camo.githubusercontent.com/915b7be44ada53c290eb157634330494ebe3e30a/68747470733a2f2f676f646f632e6f72672f6769746875622e636f6d2f676f6c616e672f6764646f3f7374617475732e737667
)](https://docs.computecoin.com/computecoin/for-developers/mainnet/)
[![Discord](https://img.shields.io/badge/discord-join%20chat-blue.svg)](https://discord.gg/f4Z2jJjtNp)

## Building the source

### Linux

Prerequisite:
* g++ version >= 5. Use command `g++ -v` to check current version.
* Install `git`, `cmake`, `wget` and `unzip`.
* cmake version >= 3.24.2. Use command `cmake --version`
  ```
  apt-get install -y build-essential git cmake wget unzip
* Install ```boost```.
  ```
  wget https://boostorg.jfrog.io/artifactory/main/release/1.66.0/source/boost_1_66_0.tar.gz
  tar --gzip -xf  boost_1_66_0.tar.gz
  cd boost_1_66_0
  ./bootstrap.sh --prefix=/usr/local
  ./b2 -j$(nproc) --with-atomic --with-chrono --with-date_time --with-filesystem --with-log \
      --with-program_options --with-regex --with-system --with-thread link=static install
  cd .. && rm -rf boost_1_66_0 boost_1_66_0.tar.gz
  ```
* Install libraries required by rocksdb. The output libraries are `liblz4.a` `libzstd.a` `libz.a`.
  ```
  apt-get install -y liblz4-dev libzstd-dev zlib1g-dev
  ```
* Install `sodium`.
  ```
  wget https://download.libsodium.org/libsodium/releases/libsodium-1.0.17-stable.tar.gz
  tar --gzip -xf  libsodium-1.0.17-stable.tar.gz
  cd libsodium-stable
  ./configure
  make -j$(nproc)
  make install
  cd .. && rm -rf libsodium-stable libsodium-1.0.17-stable.tar.gz
	```
* Install `rocksdb`. The installed library name is `librocksdb.a`.
  ```
  wget https://github.com/facebook/rocksdb/archive/v7.8.3.zip
  unzip v7.8.3.zip
  cd rocksdb-7.8.3
  PORTABLE=1 make -j$(nproc) USE_RTTI=1 static_lib
  make install
  cd .. && rm -rf rocksdb-7.8.3 v7.8.3.zip
  ```
  Note：Set `USE_RTTI=1` in Makefile. Otherwise, there will be link failed to rocksdb. Moreover, set `PORTABLE=1` to enhance the portability of the code. Please refer to the following link for more details. https://github.com/facebook/rocksdb/blob/master/INSTALL.md
	
Compile MCP

* Download source code
  ```
  git clone https://github.com/computecoin-network/mcp.git --recursive
  cd mcp && mkdir -p build && cd build
  ```
* CMake configuration
  ```
  cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_COMPILER=g++ ../
  ```
* Compile
  ```
  make -j$(nproc)
  ```
Note: If compilation fails because some libraries can not be found, first check if libraries exist in `/usr/local/lib`. Check if `make install` has been run for all installed libraries. If the libraries are installed to other directories, these directories can be specified as a `cmake` option as following.
  ```
  cmake -Dsodium_INCLUDE_DIR=/usr/local/include -Dsodium_LIBRARY_RELEASE=/usr/local/lib/libsodium.a
  ```
Next is a list of all configurable parameters in `cmake`：
  ```
  -Dsodium_INCLUDE_DIR=... -Dsodium_LIBRARY_RELEASE=.../libsodium.a -Dsodium_LIBRARY_DEBUG=.../libsodium.a
  -Drocksdb_INCLUDE_DIR=... -Drocksdb_LIBRARY_RELEASE=.../librocksdb.a -Drocksdb_LIBRARY_DEBUG=.../librocksdb.a
  -Dlz4_LIBRARY_DEBUG=.../liblz4.a -Dlz4_LIBRARY_RELEASE=.../liblz4.a
  -DZSTD_LIBRARY_DEBUG=.../libzstd.a -DZSTD_LIBRARY_RELEASE=.../libzstd.a
  -DZLIB_LIB_DEBUG=.../libz.a -DZLIB_LIB_RELEASE=.../libz.a
  ```
  
### Mac
The installation procedure is the same as Linux system. The only difference is to use command `brew install` to install packages.


### Windows

Note：1).Compilation of a few library will fail in Debug mode. Set the "treat warnings as errors" option to be false. 2). Run msbuild in the command line tools inside vs-tools, instead of in command shell.

Prerequisite:
  * Visual Studio 2022 & C++17.
  * Install `git` 、 `cmake`. Download these tools from official website, and install them to the same folder, say `C:\dependence`.
  * Install `boost`. Download Version 1.66.0 from https://www.boost.org/users/download/, and install in `C:\dependence`.
  * Install `libsodium`. Download from https://download.libsodium.org/libsodium/releases/libsodium-1.0.17-msvc.zip, and install in `C:\dependence`.
  * Build `lz4` which is required by `rocksdb`. Download from https://github.com/lz4/lz4/archive/v1.7.5.zip, and install in `C:\dependence`.
    ```
    cd c:\dependence (Download the package in this directory and unzip it)
    cd lz4-1.7.5
    cd visual\VS2010
    devenv lz4.sln /upgrade
    msbuild lz4.sln /p:Configuration=Debug /p:Platform=x64
    msbuild lz4.sln /p:Configuration=Release /p:Platform=x64
    ```	   
* Build `zstd` which is required by `rocksdb`. Download from https://github.com/facebook/zstd/archive/v1.3.7.zip, and install in `c:\dependence`.
    ```
    cd c:\dependence (Download the package in this directory and unzip it)
    cd zstd-1.3.7/build/VS2010
    devenv zstd.sln /upgrade
    msbuild zstd.sln /p:Configuration=Debug /p:Platform=x64
    msbuild zstd.sln /p:Configuration=Release /p:Platform=x64
    ```
* Build `zlib` which is required by `rocksdb`. Download from http://zlib.net/zlib1211.zip, and install in `c:\dependence`.
    ```
    cd c:\dependence (Download the package in this directory and unzip it)
    cd zlib-1.2.13\contrib\vstudio\vc10
    ```
    Edit file `zlibvc.vcxproj`，Modify `<Command>cd ..\..\contrib\masmx64` to `<Command>cd ..\..\masmx64
    ```
    devenv zlibvc.sln /upgrade
    msbuild zlibvc.sln /p:Configuration=Debug /p:Platform=x64
    msbuild zlibvc.sln /p:Configuration=Release /p:Platform=x64
    ```
* Install `rocksdb` in `c:\dependence`.
    ```
    cd c:\dependence
    ```
    Downlood using `git clone https://github.com/facebook/rocksdb` and checkout tag 7.8.3
    
    Edit `rocksdb\thirdparty.inc`.
    ```
    set(LZ4_HOME $ENV{THIRDPARTY_HOME}/lz4-1.7.5)
    set(LZ4_INCLUDE ${LZ4_HOME}/lib)
    set(LZ4_LIB_DEBUG ${LZ4_HOME}/visual/VS2010/bin/x64_Debug/liblz4_static.lib)
    set(LZ4_LIB_RELEASE ${LZ4_HOME}/visual/VS2010/bin/x64_Release/liblz4_static.lib)

    set(ZSTD_HOME $ENV{THIRDPARTY_HOME}/zstd-1.3.7)
    set(ZSTD_INCLUDE ${ZSTD_HOME}/lib ${ZSTD_HOME}/lib/dictBuilder)
    set(ZSTD_LIB_DEBUG ${ZSTD_HOME}/build/VS2010/bin/x64_Debug/libzstd_static.lib)
    set(ZSTD_LIB_RELEASE ${ZSTD_HOME}/build/VS2010/bin/x64_Release/libzstd_static.lib)

    set(ZLIB_HOME $ENV{THIRDPARTY_HOME}/zlib-1.2.13)
    set(ZLIB_INCLUDE ${ZLIB_HOME})
    set(ZLIB_LIB_DEBUG ${ZLIB_HOME}/contrib/vstudio/vc14/x64/ZlibStatDebug/zlibstat.lib)
    set(ZLIB_LIB_RELEASE ${ZLIB_HOME}/contrib/vstudio/vc14/x64/ZlibStatRelease/zlibstat.lib)
    ```
    Compile rocksdb. Follow instructions in https://github.com/facebook/rocksdb/wiki/Building-on-Windows


    ```
    mkdir build
    cd build
    set THIRDPARTY_HOME=C:\dependence
    cmake -G "Visual Studio 17 2022" -DLZ4=1 -DZLIB=1 -DZSTD=1 -DPORTABLE=1 ..
    msbuild rocksdb.sln /p:Configuration=Release
    msbuild rocksdb.sln /p:Configuration=Debug
    ```
Compile MCP

* Download source code
  ```
  git clone https://github.com/computecoin-network/mcp.git --recursive
  cd mcp && mkdir -p build && cd build
  ```
* CMake configuration. Using `cmake-gui` to help generating the configuration.
  ```
  cmake -DCMAKE_BUILD_TYPE=Release \
  -DBOOST_ROOT=C:\dependence\boost
  -DBOOST_INCLUDE_DIR=C:\dependence\boost-1_66\boost
  -Dsodium_INCLUDE_DIR=C:\dependence\lisodium\include \
  -Dsodium_LIBRARY_RELEASE=C:\dependence\lisodium\x64\Release\v141\static\libsodium.lib \
  -Dsodium_LIBRARY_DEBUG=C:\dependence\rocksdb\rocksDB\build\Debug\rocksdb.lib \
  -Drocksdb_INCLUDE_DIR=C:\dependence\rocksdb\rocksDB\include\
  -Drocksdb_LIBRARY_RELEASE=C:\dependence\build\Release\rocksdb.lib \
  -Drocksdb_LIBRARY_DEBUG=C:\dependence\rocksdb\build\Debug\rocksdb.lib \
  -Dlz4_LIBRARY_DEBUG=C:\dependence\lz4-1.7.5\visual\VS2010\bin\x64_Debug\liblz4_static.lib \
  -Dlz4_LIBRARY_RELEASE=C:\dependence\lz4-1.7.5\visual\VS2010\bin\x64_Release\liblz4_static.lib \
  -DZSTD_LIBRARY_DEBUG=C:\dependence\zstd-1.3.7\build\VS2010\bin\x64_Debug\libzstd_static.lib \
  -DZSTD_LIBRARY_RELEASE=C:\dependence\zstd-1.3.7\build\VS2010\bin\x64_Release\libzstd_static.lib \
  -DZLIB_LIB_DEBUG=C:\dependence\zlib-1.2.13\contrib\vstudio\vc10\x64\ZlibStatDebug\zlibstat.lib \
  -DZLIB_LIB_RELEASE=C:\dependence\zlib-1.2.13\contrib\vstudio\vc10\x64\ZlibStatRelease\zlibstat.lib \
  -G "Visual Studio 17 2022" ../
  ```
 * Compile the Visual Studio project
