#!/bin/bash
if ! grep -q "Slate: API Documentation Generator" README.md 2> /dev/null; then
  echo "Could not find Slate. Please run this script inside your forked repository."
  echo "See https://github.com/lord/slate#getting-started-with-slate for more details."
  exit
fi

function install {
  if [[ ! -f vendor/bundle/ruby/2.5.0/bin/middleman ]]; then
    echo "Installing missing gems..."
    bundle install --deployment || exit 1
  fi

  rm -r source/images source/includes source/index.html.md

  local rainbow=$(cd -P "$(dirname $0)/.." && pwd)
  ln -s $rainbow/doc/images source/images
  ln -s $rainbow/doc/includes source/includes
  ln -s $rainbow/doc/index.md source/index.html.md
}

case $1 in
  "build")
    install
    bundle exec middleman build --clean
    ;;
  "serve")
    install
    bundle exec middleman server
    ;;
  *)
    if [[ ! -z $1 ]]; then
      echo "Syntax: $(basename $0) [build|serve]"
    else
      $0 build
    fi
    ;;
esac
