#!/bin/sh
# Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

project_root=$(cd -P "$(dirname $0)/.." && pwd)

duktape_version=$(cat $project_root/build/cmake/Duktape.cmake | perl -wnE 'say /DUK_TARGET_VERSION ([.0-9]+)/')
duktape_hash=$(cat $project_root/build/cmake/Duktape.cmake | perl -wnE 'say /DUK_URL_HASH ([0-9a-f]+)/')
duktape_xz="duktape-${duktape_version}.tar.xz"

harfbuzz_version=$(cat $project_root/build/cmake/FreeType.cmake | perl -wnE 'say /HARFBUZZ_TARGET_VERSION ([.0-9]+)/')
harfbuzz_hash=$(cat $project_root/build/cmake/FreeType.cmake | perl -wnE 'say /HARFBUZZ_URL_HASH ([0-9a-f]+)/')
harfbuzz_bz2="harfbuzz-${harfbuzz_version}.tar.bz2"

function download {
  local name=$1
  local url=$2
  local tarball=$3
  local hash=$4
  local src=$5
  echo "${hash} *${tarball}" | shasum --algorithm 256 --check - --status
  if [[ "$?" != 0 ]]; then
    curl -LO ${url} || exit 1
    if [[ "${tarball}" == *.bz2 ]]; then
      tar xjf ${tarball}
    elif [[ "${tarball}" == *.xz ]]; then
      tar xJf ${tarball}
    else
      echo "Unknown archive type: $tarball"
      exit 1
    fi
    rm -r ${name}
    mv ${src} ${name}
  fi
}

pushd ${project_root} 1> /dev/null
command -v yarn 1> /dev/null && yarn generate-shaders

mkdir -p ${project_root}/build/ios/lib
pushd ${project_root}/build/ios/lib 1> /dev/null

download duktape \
  https://github.com/svaarala/duktape/releases/download/v${duktape_version}/${duktape_xz} \
  ${duktape_xz} \
  ${duktape_hash} \
  duktape-${duktape_version}

download harfbuzz \
  https://github.com/harfbuzz/harfbuzz/releases/download/${harfbuzz_version}/${harfbuzz_bz2} \
  ${harfbuzz_bz2} \
  ${harfbuzz_hash} \
  harfbuzz-${harfbuzz_version}

pushd harfbuzz 1> /dev/null
patch --force src/hb-coretext.cc ${project_root}/tools/harfbuzz-fix-iOS-build.patch
popd 1> /dev/null

popd 1> /dev/null
