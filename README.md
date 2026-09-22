# dehancer-external

## Build and install

```sh
cmake -S . -B build \
  -DCMAKE_INSTALL_PREFIX="$HOME/local-dehancer" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel $(nproc)
cmake --install build --config Release
```

The library remains static even when a parent sets `BUILD_SHARED_LIBS=ON`.

OpenFX plugins supply `OFX::Plugin::getPluginIDs()` when linking support code.
No tests or benchmarks are defined. `PRINT_DEBUG=ON` enables support-library logging.

`DEHANCER_EXTERNAL_INSTALL` defaults to `ON` for standalone builds and `OFF` when
embedded. Set it explicitly to override installation of the library, headers,
and CMake package.

## CMake consumption

Installed package:

```cmake
find_package(dehancer_external CONFIG REQUIRED)
target_link_libraries(my_plugin PRIVATE dehancer_external::dehancer_external)
```

Configure consumers with `-DCMAKE_PREFIX_PATH=/path/to/prefix`.

Source checkout:

```cmake
add_subdirectory(path/to/dehancer-external)
target_link_libraries(my_plugin PRIVATE dehancer_external::dehancer_external)
```

FetchContent with a local checkout:

```cmake
include(FetchContent)
FetchContent_Declare(dehancer_external
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/dehancer-external"
)
FetchContent_MakeAvailable(dehancer_external)
target_link_libraries(my_plugin PRIVATE dehancer_external::dehancer_external)
```

The target propagates C++17 and header paths. Both `<openfx/ofxsImageEffect.h>`
and `<ofxsImageEffect.h>` work in all three modes. Use `PUBLIC` linkage when your
public headers expose OpenFX types. Parents can supply this target directly;
consumers should call `find_package` only when the target does not exist.
