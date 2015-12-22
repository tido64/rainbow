import os
import re
import subprocess
import sys

# Copied from Chromium. See https://codereview.chromium.org/842053003
def SystemIncludeDirectoryFlags():
  """Determines compile flags to include the system include directories.

  Use as a workaround for https://github.com/Valloric/YouCompleteMe/issues/303

  Returns:
    (List of Strings) Compile flags to append.
  """
  try:
    with open(os.devnull, 'rb') as DEVNULL:
      output = subprocess.check_output(['clang', '-v', '-E', '-x', 'c++', '-'],
                                       stdin=DEVNULL, stderr=subprocess.STDOUT)
  except (FileNotFoundError, subprocess.CalledProcessError):
    return []
  includes_regex = r'#include <\.\.\.> search starts here:\s*' \
                   r'(.*?)End of search list\.'
  includes = re.search(includes_regex, output.decode(), re.DOTALL).group(1)
  flags = []
  for path in includes.splitlines():
    path = path.strip()
    if os.path.isdir(path):
      flags.append('-isystem')
      flags.append(path)
  return flags

def ProjectFlags():
  project_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
  flags = [
    '-std=c++14',
    '-Werror',
    '-pedantic',
    '-Wall',
    '-Wextra',
    '-Weffc++',
    '-Wold-style-cast',
    '-Woverloaded-virtual',
    '-Wsign-promo',
    '-I', os.path.join(project_path, 'src'),
    '-I', os.path.join(project_path, 'src', 'ThirdParty', 'FreeType'),
    '-I', os.path.join(project_path, 'src', 'ThirdParty', 'libpng'),
    '-I', os.path.join(project_path, 'lib'),
    '-I', os.path.join(project_path, 'lib', 'box2d', 'Box2D'),
    '-I', os.path.join(project_path, 'lib', 'FMOD', 'inc'),
    '-I', os.path.join(project_path, 'lib', 'FreeType', 'include'),
    '-I', os.path.join(project_path, 'lib', 'Lua'),
    '-I', os.path.join(project_path, 'lib', 'SDL', 'include'),
    '-I', os.path.join(project_path, 'lib', 'libpng'),
    '-I', os.path.join(project_path, 'lib', 'nanovg', 'src'),
    '-I', os.path.join(project_path, 'lib', 'spine-runtimes', 'spine-c', 'include'),
    '-I', os.path.join(project_path, 'lib', 'googletest', 'googletest', 'include'),
    '-DUSE_FMOD_STUDIO=1',
    '-DUSE_PHYSICS=1',
    '-DUSE_SPINE=1',
    '-fno-rtti',
    '-fno-exceptions',
  ]
  flags.extend(SystemIncludeDirectoryFlags())
  return flags

cxx_flags = ProjectFlags()

def FlagsForFile(file):
  global cxx_flags
  return { 'flags': cxx_flags, 'do_cache': True }
