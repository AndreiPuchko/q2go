#!/bin/bash
zlib_version="zlib-1.3.1"
if ! test -d $zlib_version; then
  echo https://zlib_version.net/${zlib_version}.tar.gz
  curl -s  https://zlib.net/${zlib_version}.tar.gz | tar -xz -C .
else
  echo "${zlib_version} is already here"
fi


if ! test -d quazip; then
  git clone -b master --depth 1 --single-branch https://github.com/stachenov/quazip.git
  rm -rf quazip/.git*
else
  echo quazip is already here
fi
