#!/usr/bin/env bash
_setup_boost_deps(){
  local DEP_NAME=$1
  local DEP_REVISION=$2
  local DEP_CHECK=$3
  local DEP_MODULES=$4
  local DEP_REVISION_US=$(echo $DEP_REVISION|sed -e "s/\./_/g")
  local DEP_URL="https://archives.boost.io/release/${DEP_REVISION}/source/boost_${DEP_REVISION_US}.tar.gz"
  printf "[%s][%s][%s][%s]\n" ${DEP_NAME} ${DEP_URL} ${DEP_REVISION} ${DEP_MODULES}
  local DEP_SRC=${APP_DEPS_DIR}/src
  mkdir -p "${DEP_SRC}"
  local DEP_SRC_APP=${DEP_SRC}/${DEP_NAME}
  local DEP_SRC_APP_BUILD=${DEP_SRC}/${DEP_NAME}_build
  local DEP_SRC_TGZ=${DEP_SRC}/${DEP_NAME}-${DEP_REVISION}.tar.gz
  if [[ ! -f $DEP_SRC_TGZ ]];then
    wget -O ${DEP_SRC_TGZ} ${DEP_URL}
  fi
  local BOOST_SRC=$DEP_SRC_APP/boost_${DEP_REVISION_US}
  if [[ ! -f ${BOOST_SRC}/b2 ]];then
    mkdir -p ${DEP_SRC_APP}
    tar -zxvf ${DEP_SRC_TGZ} -C ${DEP_SRC_APP}
  fi
  BOOST_MODULES_FILE=${BOOST_SRC}/.boost_modules
  EXIST_MODULES=""
  if [[ -f ${BOOST_MODULES_FILE} ]];then
    EXIST_MODULES=$(cat ${BOOST_MODULES_FILE})
  fi
  if [[ ! -f $APP_INSTALL_DIR/${DEP_CHECK} || "$DEP_MODULES" != "$EXIST_MODULES" ]];then
    pushd "${BOOST_SRC}" || exit
    ./bootstrap.sh --prefix=$APP_INSTALL_DIR --with-libraries=$DEP_MODULES
    ./b2 cxxflags=-fPIC cflags=-fPIC install
    echo "${DEP_MODULES}" > $BOOST_MODULES_FILE
    popd || exit
  fi
}


_setup_cmake_deps(){
  local DEP_NAME=""
  local DEP_URL=""
  local DEP_REVISION=""
  local DEP_CHECK=""
  local DEP_MODULES=""
  local SUB_DIR=""

  while [[ $# -gt 0 ]]; do
      case "$1" in
          -n|--name)
              DEP_NAME=$2
              shift 2
              ;;
          --sub)
              SUB_DIR=$2
              shift 2
              ;;
          -u|--url)
              DEP_URL=$2
              shift 2
              ;;
          -v|--revision)
              DEP_REVISION=$2
              shift 2
              ;;
          -c|--check)
              DEP_CHECK=$2
              shift 2
              ;;
          -m|--modules)
              DEP_MODULES=$2
              shift 2
              ;;
          --)
              shift 1
              break
              ;;
          -h|--help)
              echo "-n|--name value"
              echo "-u|--url value"
              echo "-v|--revision value"
              echo "-c|--check value"
              echo "-m|--modules value"
              echo "-- args"
              shift
              exit 0
              ;;
          *)
              echo "Unknown option: $1"
              exit 1
              ;;
      esac
  done
  printf "[%s][%s][%s]\n" ${DEP_NAME} ${DEP_URL} ${DEP_REVISION}
  local DEP_SRC=${APP_DEPS_DIR}/src
  local DEP_SRC_APP=${DEP_SRC}/${DEP_NAME}
  local DEP_SRC_APP_BUILD=${DEP_SRC}/${DEP_NAME}_build
  if [[ ! -d $DEP_SRC_APP ]];then
    mkdir -p "$DEP_SRC"
    pushd "$DEP_SRC" || exit
    git clone --recursive "$DEP_URL" -b "${DEP_REVISION}" "${DEP_NAME}"
    popd || exit
  fi
  local build_fn=build_${DEP_NAME}
  if [[ ! -f $APP_INSTALL_DIR/${DEP_CHECK} ]];then
    mkdir -p "$DEP_SRC_APP_BUILD"
    pushd "$DEP_SRC_APP_BUILD" || exit
    if type "${build_fn}" >/dev/null 2>&1;then
      export DEP_SRC_APP
      export DEP_SRC_APP_BUILD
      eval "${build_fn}"
    else
      $CMAKE_BIN \
        -DCMAKE_PREFIX_PATH="${APP_INSTALL_DIR}" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${APP_INSTALL_DIR}" $@ "${DEP_SRC_APP}${SUB_DIR}"
      make -j"$(nproc)"
      make install
    fi
    popd || exit
  fi
}
build_talib(){
  pushd "${DEP_SRC_APP}" || exit
  if [[ ! -f configure ]];then
    bash autogen.sh
  fi
  ./configure --prefix="${APP_INSTALL_DIR}"
  make -j"$(nproc)"
  make install
  popd || exit
}
build_libunwind(){
  pushd "${DEP_SRC_APP}" || exit
  if [[ ! -f configure ]];then
    autoreconf -i
  fi
  ./configure --prefix="${APP_INSTALL_DIR}" CFLAGS='-fPIC -O3' CXXFLAGS='-fPIC -O3'
  make -j"$(nproc)"
  make install
  popd || exit
}
build_protobuf(){
  pushd "${DEP_SRC_APP_BUILD}" || exit
  cmake -DCMAKE_INSTALL_PREFIX="${APP_INSTALL_DIR}" \
    -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF \
    -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS="-fPIC" ${DEP_SRC_APP}/cmake
  make -j"$(nproc)"
  make install
  popd || exit
}
build_zsdt(){
  pushd "${DEP_SRC_APP}" || exit
  make -j"$(nproc)"
  make install PREFIX="${APP_INSTALL_DIR}"
  popd || exit
}
build_tcmalloc(){
  require bazel
  pushd "${DEP_SRC_APP}" || exit
  bazel version
  bazel build //tcmalloc:tcmalloc
  cp bazel-bin/tcmalloc/libtcmalloc.lo "${APP_INSTALL_DIR}"/lib/libtcmalloc.lo
  popd || exit
}


start_deps(){
  mkdir -p "$APP_DEPS_DIR"
  #_setup_boost_deps boost 1.86.0 include/boost/config.hpp iostreams,random,filesystem
  _setup_cmake_deps -n abseil-cpp -u https://github.com/abseil/abseil-cpp.git -v lts_2024_07_22 -c include/absl/base/config.h -- \
   -DABSL_BUILD_TESTING=OFF -DABSL_USE_GOOGLETEST_HEAD=ON -DCMAKE_CXX_STANDARD=17 -DABSL_INTERNAL_AT_LEAST_CXX17=ON
  #_setup_cmake_deps -n cli -u https://github.com/daniele77/cli.git -v v2.2.0 -c include/cli/cli.h -- \
  #   -DCLI_BuildExamples=OFF
  _setup_cmake_deps -n gtest -u https://github.com/google/googletest.git -v v1.15.2 -c include/gtest/gtest.h -- \
   -DCMAKE_CXX_FLAGS="-fPIC"
  _setup_cmake_deps -n zlib -u https://github.com/madler/zlib.git -v v1.3.1 -c include/zlib.h -- \
    -DZLIB_BUILD_EXAMPLES=OFF

  _setup_cmake_deps -n libunwind -u https://github.com/libunwind/libunwind.git -v v1.8.1 -c include/unwind.h
  _setup_cmake_deps -n gflags -u https://github.com/gflags/gflags.git -v v2.2.2 -c include/gflags/gflags.h -- \
   -DBUILD_STATIC_LIBS=ON -DCMAKE_CXX_FLAGS="-fPIC"
  _setup_cmake_deps -n glog -u https://github.com/google/glog.git -v v0.6.0 -c include/glog/logging.h -- \
   -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_CXX_FLAGS="-fPIC"

  #_setup_cmake_deps -n talib -u https://github.com/TA-Lib/ta-lib.git -v main -c include/ta-lib/ta_common.h
  #_setup_cmake_deps -n hdf5 -u https://github.com/HDFGroup/hdf5.git -v hdf5_1.14.5 -c include/hdf5.h
  #_setup_cmake_deps -n blosc2 -u https://github.com/Blosc/c-blosc2.git -v v2.15.1 -c include/blosc2.h -- \
  #  -DBUILD_SHARED=OFF
  _setup_cmake_deps -n xz -u https://github.com/tukaani-project/xz.git -v v5.6.3 -c include/lzma.h -- \
    -DCMAKE_C_FLAGS_RELEASE="-fPIC"
  #_setup_cmake_deps -n protobuf -u https://github.com/protocolbuffers/protobuf.git -v v3.19.1 -c include/google/protobuf/message.h -- \
  #   -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS="-fPIC"

  ## json
  _setup_cmake_deps -n rapidjson -u https://github.com/Tencent/rapidjson.git -v v1.1.0 -c include/rapidjson/rapidjson.h -- \
    -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF
  ## yaml
  _setup_cmake_deps -n yaml -u https://github.com/jbeder/yaml-cpp.git -v yaml-cpp-0.7.0 -c include/yaml-cpp/yaml.h -- \
   -DYAML_BUILD_SHARED_LIBS=OFF -DYAML_CPP_BUILD_TESTS=OFF

  _setup_cmake_deps -n zsdt -u https://github.com/facebook/zstd.git -v v1.5.6 -c include/zstd.h
  _setup_cmake_deps -n boringssl -u https://github.com/google/boringssl.git -v 0.20241203.0 -c include/openssl/base.h

  #_setup_cmake_deps -n snappy -u https://github.com/google/snappy.git -v 1.2.1 -c include/snappy.h -- \
  #  -DSNAPPY_BUILD_TESTS=OFF -DSNAPPY_BUILD_BENCHMARKS=OFF -DCMAKE_CXX_FLAGS="-fPIC"
  #_setup_cmake_deps -n leveldb -u https://github.com/google/leveldb.git -v 1.23 -c include/leveldb/db.h -- \
  #   -DBUILD_SHARED_LIBS=OFF -DLEVELDB_BUILD_TESTS=OFF -DLEVELDB_BUILD_BENCHMARKS=OFF -DCMAKE_CXX_FLAGS="-fPIC"
  #_setup_cmake_deps -n brpc -u https://github.com/apache/brpc.git -v 1.11.0 -c include/brpc/server.h -- \
  #    -DBUILD_STATIC_LIBS=ON -DWITH_DEBUG_SYMBOLS=OFF -DWITH_GLOG=ON
  #_setup_cmake_deps -n arrow -u https://github.com/apache/arrow.git -v apache-arrow-18.1.0 --sub /cpp -- \
  #  -DARROW_BUILD_TESTS=OFF -DARROW_PARQUET=ON
}
