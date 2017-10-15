#!/bin/sh
# Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

project_root=$(cd -P "$(dirname $0)/.." && pwd)
duktape_version=$(cat $project_root/build/cmake/Duktape.cmake | perl -wnE 'say /DUK_TARGET_VERSION ([.0-9]+)/')
duktape_hash=$(cat $project_root/build/cmake/Duktape.cmake | perl -wnE 'say /DUK_URL_HASH ([0-9a-f]+)/')
duktape_xz="duktape-${duktape_version}.tar.xz"

pushd ${project_root} 1> /dev/null
tools/shaders-gen.py

mkdir -p ${project_root}/build/ios/lib
pushd ${project_root}/build/ios/lib 1> /dev/null

echo "${duktape_hash} *${duktape_xz}" | shasum --algorithm 256 --check - --status
if [[ "$?" != 0 ]]; then
  curl -LO https://github.com/svaarala/duktape/releases/download/v${duktape_version}/${duktape_xz} || exit 1
  tar xJf ${duktape_xz}
  mv duktape-${duktape_version} duktape
fi

popd 1> /dev/null
