## Olympus (The blockchain layer in Oort)
Official C++ implementation of the Olympus protocol.

To understand the consensus algorithm in Olympus, refer to the mcp [yellow paper](https://resources.computecoin.com/docs/computecoin-consensus-and-security.pdf).

[![API Reference](
https://camo.githubusercontent.com/915b7be44ada53c290eb157634330494ebe3e30a/68747470733a2f2f676f646f632e6f72672f6769746875622e636f6d2f676f6c616e672f6764646f3f7374617475732e737667
)](https://docs.oortech.com/oort/community/developers/blockchain-developers/)
[![Discord](https://img.shields.io/badge/discord-join%20chat-blue.svg)](https://discord.gg/f4Z2jJjtNp)

## Building the source

### Linux

Prerequisite:
* g++ version >= 9. Use command `g++ -v` to check current version.
* Install `git`, `cmake`, `wget` and `unzip`.
  ```
  apt-get install -y git cmake wget unzip
* Install ```boost```.
  ```
  wget https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0_rc1.tar.bz2
  tar --bzip2 -xf  boost_1_81_0_rc1.tar.bz2
  cd boost_1_81_0
  ./bootstrap.sh --prefix=/usr/local
  ./b2 -j$(nproc) --with-atomic --with-chrono --with-date_time --with-filesystem --with-log \
      --with-program_options --with-regex --with-system --with-thread link=static install
  cd .. && rm -rf boost_1_81_0 boost_1_81_0_rc1.tar.bz2
  ```
* Install libraries required by rocksdb. The output libraries are `liblz4.a` `libzstd.a` `libz.a`.
  ```
  apt-get install -y liblz4-dev libzstd-dev zlib1g-dev
  ```
* Install `rocksdb`. The installed library name is `librocksdb.a`.
  ```
  wget https://github.com/facebook/rocksdb/archive/v8.3.3.zip
  unzip v8.3.3.zip
  cd rocksdb-8.3.3
  PORTABLE=1 make -j$(nproc) USE_RTTI=1 static_lib
  make install
  cd .. && rm -rf rocksdb-8.3.3 v8.3.3.zip
  ```
  Note: Set `USE_RTTI=1` in Makefile. Otherwise, there will be link failed to rocksdb. Moreover, set `PORTABLE=1` to enhance the portability of the code. Please refer the [Installation Instructions](https://github.com/facebook/rocksdb/blob/master/INSTALL.md) for more details.
	
Compile MCP

* Download source code
  ```
  git clone https://github.com/oort-tech/Olympus.git --recursive
  cd Olympus
  mkdir -p build && cd build
  ```
* CMake configuration
  ```
  cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../
  ```
* Compile
  ```
  make -j$(nproc)
  ```
Note: If compilation fails because some libraries can not be found, first check if libraries exist in `/usr/local/lib`. Check if `make install` has been run for all installed libraries. If the libraries are installed to other directories, these directories can be specified as a `cmake` option as following.
  ```
  cmake -Drocksdb_INCLUDE_DIR=/usr/local/include -Drocksdb_LIBRARY_RELEASE=/usr/local/lib/librocksdb.a
  ```
Next is a list of all configurable parameters in `cmake`：
  ```
  -DBOOST_ROOT=...
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
  * Visual studio version >= 2022.
  * Install `git` 、 `cmake`. Download these tools from official website.
  * Install `boost`. Download Version 1.81.0 from [boost.org](https://www.boost.org/users/history/version_1_81_0.html), and install in `c:\dependence`.
  * Install `lz4` which is required by `rocksdb`. Download [v1.9.2](https://github.com/lz4/lz4/archive/refs/tags/v1.9.2.zip), and install in `c:\dependence`.
    ```
    cd c:\dependence (Download the package in this directory and unzip it)
    cd lz4-1.9.2
    cd visual\VS2017
    devenv lz4.sln /upgrade
    msbuild lz4.sln /p:Configuration=Debug /p:Platform=x64
    msbuild lz4.sln /p:Configuration=Release /p:Platform=x64
    ```	   
* Install `zstd` which is required by `rocksdb`. Download [v1.5.2](https://github.com/facebook/zstd/archive/v1.5.2.zip), and install in `c:\dependence`.
    ```
    cd c:\dependence (Download the package in this directory and unzip it)
    cd zstd-1.5.2\build\VS2010
    devenv zstd.sln /upgrade
    msbuild zstd.sln /p:Configuration=Debug /p:Platform=x64
    msbuild zstd.sln /p:Configuration=Release /p:Platform=x64
    ```
* Install `zlib` which is required by `rocksdb`. Download [v1.2.13](https://github.com/madler/zlib/archive/refs/tags/v1.2.13.zip), and install in `c:\dependence`.
    ```
    cd c:\dependence (Download the package in this directory and unzip it)
    cd zlib-1.2.13\contrib\vstudio\vc14
    devenv zlibvc.sln /upgrade
    msbuild zlibvc.sln /p:Configuration=Debug /p:Platform=x64
    msbuild zlibvc.sln /p:Configuration=Release /p:Platform=x64
    ```
* Install `rocksdb` in `c:\dependence`.
    ```
    cd c:\dependence
    ```
    Download [rocksdb](https://github.com/facebook/rocksdb/archive/refs/tags/v8.3.3.zip) and unzip it.
    
    Edit `rocksdb\thirdparty.inc`.
    ```
    set(LZ4_HOME $ENV{THIRDPARTY_HOME}/lz4-1.9.2)
    set(LZ4_INCLUDE ${LZ4_HOME}/lib)
    set(LZ4_LIB_DEBUG ${LZ4_HOME}/visual/VS2017/bin/x64_Debug/liblz4_static.lib)
    set(LZ4_LIB_RELEASE ${LZ4_HOME}/visual/VS2017/bin/x64_Release/liblz4_static.lib)

    set(ZSTD_HOME $ENV{THIRDPARTY_HOME}/zstd-1.5.2)
    set(ZSTD_INCLUDE ${ZSTD_HOME}/lib ${ZSTD_HOME}/lib/dictBuilder)
    set(ZSTD_LIB_DEBUG ${ZSTD_HOME}/build/VS2010/bin/x64_Debug/libzstd_static.lib)
    set(ZSTD_LIB_RELEASE ${ZSTD_HOME}/build/VS2010/bin/x64_Release/libzstd_static.lib)

    set(ZLIB_HOME $ENV{THIRDPARTY_HOME}/zlib-1.2.13)
    set(ZLIB_INCLUDE ${ZLIB_HOME})
    set(ZLIB_LIB_DEBUG ${ZLIB_HOME}/contrib/vstudio/vc14/x64/ZlibStatDebug/zlibstat.lib)
    set(ZLIB_LIB_RELEASE ${ZLIB_HOME}/contrib/vstudio/vc14/x64/ZlibStatRelease/zlibstat.lib)
    ```
    Compile rocksdb. Follow [Installation Instructions](https://github.com/facebook/rocksdb/wiki/Building-on-Windows).


    ```
    mkdir build
    cd build
    set THIRDPARTY_HOME=c:\dependence(cmd) or $env:THIRDPARTY_HOME="c:\dependence" (powershell)
    cmake -G "Visual Studio 17 2022" -DLZ4=1 -DZLIB=1 -DZSTD=1 -DPORTABLE=1 ..
    msbuild rocksdb.sln /p:Configuration=Release
    msbuild rocksdb.sln /p:Configuration=Debug
    ```
Compile MCP

* Download source code
  ```
  git clone https://github.com/oort-tech/Olympus.git --recursive
  cd mcp && mkdir -p build && cd build
  cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBOOST_ROOT=c:\dependence\boost \
  -Drocksdb_INCLUDE_DIR=c:\dependence\rocksdb\include \
  -Drocksdb_LIBRARY_RELEASE=c:\dependence\rocksdb\build\Release\rocksdb.lib \
  -Drocksdb_LIBRARY_DEBUG=c:\dependence\rocksdb\build\Debug\rocksdb.lib \
  -Dlz4_LIBRARY_DEBUG=c:\dependence\lz4-1.7.5\visual\VS2010\bin\x64_Debug\liblz4_static.lib \
  -Dlz4_LIBRARY_RELEASE=c:\dependence\lz4-1.7.5\visual\VS2010\bin\x64_Release\liblz4_static.lib \
  -DZSTD_LIBRARY_DEBUG=c:\dependence\zstd-1.3.7\build\VS2010\bin\x64_Debug\libzstd_static.lib \
  -DZSTD_LIBRARY_RELEASE=c:\dependence\zstd-1.3.7\build\VS2010\bin\x64_Release\libzstd_static.lib \
  -DZLIB_LIB_DEBUG=c:\dependence\zlib-1.2.11\contrib\vstudio\vc14\x64\ZlibStatDebug\zlibstat.lib \
  -DZLIB_LIB_RELEASE=c:\dependence\zlib-1.2.11\contrib\vstudio\vc14\x64\ZlibStatRelease\zlibstat.lib \
  -G "Visual Studio 17 2022" ../
  msbuild mcp.sln /p:Configuration=Debug /p:Platform=x64
  msbuild mcp.sln /p:Configuration=Release /p:Platform=x64
  ```

## Run a node
### Read Help
./mcp --help
### Run a node as an ordinary node
./mcp --daemon --console --data_path=\<Your data path\>
### Run a node as a witness
./mcp --daemon --console --data_path=\<Your data path\> --witness --witness_account=\<Witness account keystore file\> --password=\<Password for the keystore file\>

Once the mcp is running, you can change other configuration items in the config.json file in the data path specified. 
