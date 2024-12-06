#!/bin/bash
# https://git-scm.com/docs/pretty-formats
start_gen_git_revision(){
  require git
  BRANCH=$(git rev-parse --abbrev-ref HEAD)
  
  COMMIT=$(git rev-parse --short HEAD)
  
  FULL_COMMIT=$(git rev-parse HEAD)
  
  TIME=$(git show --no-patch --format="%ci" HEAD)
  
  MSG=$(git show -s --format="%h:%an:%s %ar" -1)
  
  REF=$(git describe --always --tags --dirty)
  
  LAST_COMMIT_USER=$(git log -1 --pretty=format:'%an')

cat <<EOF > "${BUILD_DIR}/git_version.h"
#pragma once
// this file is auto-generate by $(basename $0),don't edit it
// git version:
#define GIT_REF "$REF"
#define GIT_BRANCH "$BRANCH"
#define GIT_COMMIT "$FULL_COMMIT"
#define GIT_SHORT_COMMIT "$COMMIT"
#define GIT_DATE "$TIME"
#define GIT_LAST_MSG "$MSG"
#define GIT_LAST_COMMIT_USER "$LAST_COMMIT_USER"
EOF

}
