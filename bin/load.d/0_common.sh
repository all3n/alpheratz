#!/bin/bash
get_os_name() {
    if [ "$(uname)" == "Darwin" ]; then
        echo "macOS"
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        echo "Linux"
    elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ] || [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
        echo "Windows"
    else
        echo "Unknown"
    fi
}

get_mach(){
  uname -m
}

log() { 
  local DT=$(date "+%F %T")
  printf "[${DT}] $*\n";
}

exit_if_err() {
  local err_code=$?
  if (( err_code )); then
    log "$*"
    exit $err_code
  fi
}



has(){
  which "$1" > /dev/null 2>&1
}

require(){
  has "$1"
  exit_if_err "require $1"
}
