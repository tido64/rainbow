import os
import sys

def PlatformIncludes():
  if sys.platform.startswith('linux'):
    # TODO
    return []
  elif sys.platform == 'darwin':
    # Requires Xcode 5.1+ and YouCompleteMe compiled with '--clang-completer --system-libclang'
    return [
      '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/c++/v1',
      '/usr/local/include',
      '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/5.1/include',
      '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include',
      '/usr/include',
    ]
  return []

def ProjectFlags():
  project_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
  flags = [
    '-std=c++11',
    '-Werror',
    '-pedantic',
    '-Wall',
    '-Wextra',
    '-Weffc++',
    '-Wold-style-cast',
    '-Woverloaded-virtual',
    '-Wsign-promo',
    '-I', os.path.join(project_path, 'src'),
    '-I', os.path.join(project_path, 'lib'),
    '-I', os.path.join(project_path, 'lib', 'FMOD', 'inc'),
    '-I', os.path.join(project_path, 'src', 'ThirdParty', 'FreeType'),
    '-I', os.path.join(project_path, 'lib', 'FreeType', 'include'),
    '-I', os.path.join(project_path, 'lib', 'Lua'),
    '-I', os.path.join(project_path, 'lib', 'SDL', 'include'),
    '-I', os.path.join(project_path, 'src', 'ThirdParty', 'libpng'),
    '-I', os.path.join(project_path, 'lib', 'libpng'),
    '-DUSE_FMOD_STUDIO=1',
    '-fno-rtti',
    '-fno-exceptions',
  ]
  # Workaround for https://github.com/Valloric/YouCompleteMe/issues/303
  for path in PlatformIncludes():
    flags.append('-isystem')
    flags.append(path)
  return flags

cxx_flags = ProjectFlags()

def FlagsForFile(file):
  global cxx_flags
  return { 'flags': cxx_flags, 'do_cache': True }
