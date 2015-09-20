# Coding Standard

## Header Files

Filenames must end in `.h`, and should be self-contained. That means that you
should be able to simply include it without having to include or declare
anything else.

### Header Guards

All header files must have `#define` guards. The format of the macro should be
the full path of the file starting from `src/`. E.g. the file
`src/Common/impl/DataMap_Unix.h` should have the following guard:

```c++
#ifndef COMMON_IMPL_DATAMAP_UNIX_H_
#define COMMON_IMPL_DATAMAP_UNIX_H_
…
#endif
```

### Forward Declarations

Use forward declarations whenever possible to avoid unnecessary `#include`s.

### Names and Order of Includes

1. Corresponding header
2. C/C++ library
3. Third-party libraries
4. Rainbow headers

Within each section, order the includes alphabetically. The paths to Rainbow's
header files must always be descendants of `src/`. Don't use relative paths, or
paths with `.` or `..`.

For example, see `src/ThirdParty/Spine/spine-rainbow.cpp`:

```c++
#include "ThirdParty/Spine/spine-rainbow.h"

#include <algorithm>

#include <spine/SkeletonJson.h>
#include <spine/extension.h>

#include "Common/DataMap.h"
#include "FileSystem/Path.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/TextureAtlas.h"
```

## Scoping

### Namespaces

- Use unnamed namespaces in your `.cpp`s.
- _using-directives_, e.g. `using namespace std;`, is forbidden.
- _using-declarations_, e.g. `using std::vector;`, are allowed anywhere in
  `.cpp`s, and inside classes, functions, methods, and namespaces in `.h`s.

### Freestanding Functions

- Declare freestanding functions within the `rainbow` namespace, or as static
  member functions. Global functions are not allowed.

### Local Variables

- Initialise variables in declaration, as close to the site of first use as
  possible, and in the most limiting scope possible.
- Avoid static and global variables unless they can be declared `const` or
  `constexpr`. Otherwise, use the `Global` template for classes that should be
  globally accessible.

## Classes

- Always initialise members in the constructor using an initialisation list.
- Use the `explicit` keyword for constructors callable with one argument.
- Prefer composition over inheritance.
- Data members should be private. If access to them are needed, define
  accessors and mutators.

### Declaration Order

Declare public members first, followed by protected, and finally private.
Within each section, use the following order:

1. `using`s, enums, and inner classes/structs
2. Constants (static const data members)
3. Static methods
4. Data members (except static const data members)
5. Constructors
6. Destructor
7. Accessors and mutators
8. Methods
9. Event handlers
10. Operator overloads
11. Overrides

## C++ Features

- Don't use C++ run-time type information.
- Don't use C++ exceptions.
- Use explicit casts like `static_cast<>()` and `reinterpret_cast<>()`. Old
  C-style cast is forbidden.
- Use `constexpr` and `const` "everywhere".
- Use macros sparingly. If you cannot avoid them, try to `#define` them just
  before usage site and `#undef` right after.
- Use `0` for integers, `0.0` for reals, `nullptr` for pointers, and `\0` for
  chars.
- Prefer `sizeof()` on the instance itself rather than the type.
- Use `final` and `override` when overriding methods.
- Declare derived classes `final` whenever possible.

## Naming Conventions

- File names are **TitleCased** and suffixed `.cpp` or `.h` for C++ files.
  - Corresponding test files are suffixed `.test.cc`.
- Class names are **TitleCased**.
  - Abstract interfaces must have an **I** prefixed.
- Variable names are **snake_cased**.
  - Member variables have a trailing underscore, e.g. `a_local_variable_`.
  - Struct members don't have a trailing underscore.
- Constant names are **TitleCased** and are prefixed with **k**.
- Function names are **snake_cased**.
  - Member accessors are named after the backing variable without the trailing
    underscore, e.g. `int status() const { return status_; }`.
  - Member setters are named after the backing variable with **set_**
    prefixed, e.g. `void set_status(int status) { status_ = status; }`.
- Namespace names are **snake_cased**.
- Enumerator names are **TitleCased**.
- Macro names are all **UPPER_CASE_AND_UNDERSCORES**.

## Formatting

Use the provided `.clang-format` configuration to format any written code. There
should be a [ClangFormat](http://clang.llvm.org/docs/ClangFormat.html) plugin
for your favourite editor:

- Sublime Text: [Clang Format](https://github.com/rosshemsley/SublimeClangFormat)
- Visual Studio: [clang-format plugin](http://llvm.org/builds/)
- Vim: [vim-clang-format](https://github.com/rhysd/vim-clang-format)
- Xcode: [ClangFormat-Xcode](https://github.com/travisjeffery/ClangFormat-Xcode)

## Other Guidelines

- [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines)
- [Google C++ Style Guide](https://google-styleguide.googlecode.com/svn/trunk/cppguide.html)
- [LLVM Coding Standards](http://llvm.org/docs/CodingStandards.html)
- [Unreal Engine Coding Standard](https://docs.unrealengine.com/latest/INT/Programming/Development/CodingStandard/)
