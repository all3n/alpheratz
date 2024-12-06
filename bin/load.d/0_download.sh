#!/usr/bin/env bash
# this utils function for download


download_pkg(){
  local PKG_URL="$1"
  local INSTALL_DIR="$2"
  local PKG_NAME=$(base "${PKG_URL}")
  local DOWNLOAD_PKG_DIR="${APP_DEPS_DIR}/pkgs"
  mkdir -p "$DOWNLOAD_PKG_DIR"
  local DOWNLOAD_LOCAL_PATH="${DOWNLOAD_PKG_DIR}/${PKG_NAME}"
  if [[ -f ${DOWNLOAD_LOCAL_PATH} ]];then
    echo "${DOWNLOAD_LOCAL_PATH} exists, skip download"
    return
  fi
  local INSTALL_PATH="${APP_INSTALL_DIR}/${INSTALL_DIR}"
  if [[ ! -d ${INSTALL_PATH} ]];then
    mkdir -p "${INSTALL_PATH}"
  fi
}
