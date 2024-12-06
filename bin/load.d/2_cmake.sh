
get_cmake_home(){
  if [[ -d ${APP_DEPS_DIR} ]];then
    echo $(find ${APP_DEPS_DIR} -maxdepth 1 -type d -name 'cmake-*' -print)
  fi
}
start_cmake(){
  local OS_NAME=$(get_os_name)
  local MACH_NAME=$(get_mach)
  if [[ "${OS_NAME}" == "macOS" ]];then
    CMAKE_TGZ_URL=$(get_latest_archive Kitware/CMake macos-universal.tar.gz)
  else
    CMAKE_TGZ_URL=$(get_latest_archive Kitware/CMake linux-${MACH_NAME}.tar.gz)
  fi
  CMAKE_DIR=$(get_cmake_home)
  if [[ -z "$CMAKE_DIR" ]];then
    TGZ_LOCAL=${APP_DEPS_DIR}/cmake.tar.gz
    wget -O $TGZ_LOCAL ${CMAKE_TGZ_URL}
    tar -zxvf $TGZ_LOCAL -C $APP_DEPS_DIR
  fi
  CMAKE_DIR=$(get_cmake_home)
  $CMAKE_DIR/bin/cmake --version |grep version
}


detect_cmake_bin(){
  CMAKE_HOME=$(get_cmake_home)
  if [[ -n "$CMAKE_HOME" ]];then
    echo ${CMAKE_HOME}/bin/cmake
  else
    echo cmake
  fi
}
