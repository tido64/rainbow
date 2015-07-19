#!/bin/sh
RAINBOW=$(cd -P "$(dirname $0)/../src" && pwd)
nim cpp --path:$RAINBOW/Nim --app:staticlib --header --noMain --cincludes:$RAINBOW $@ main.nim
