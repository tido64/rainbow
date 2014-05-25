import os

project_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
cxx_flags = [
  "-std=c++11",
  "-Werror",
  "-pedantic",
  "-Wall",
  "-Wextra",
  "-Weffc++",
  "-Wold-style-cast",
  "-Woverloaded-virtual",
  "-Wsign-promo",
  "-I" + os.path.join(project_path, "src"),
  "-I" + os.path.join(project_path, "lib"),
  "-I" + os.path.join(project_path, "src", "ThirdParty", "FreeType"),
  "-I" + os.path.join(project_path, "lib", "FreeType", "include"),
  "-I" + os.path.join(project_path, "lib", "Lua"),
  "-I" + os.path.join(project_path, "lib", "SDL", "include"),
  "-I" + os.path.join(project_path, "src", "ThirdParty", "libpng"),
  "-I" + os.path.join(project_path, "lib", "libpng"),
  "-fno-rtti",
  "-fno-exceptions",
]

def FlagsForFile(file):
  return { 'flags': cxx_flags, 'do_cache': True }
