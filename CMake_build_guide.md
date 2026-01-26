# CMake Build Guide - NXP ZBOSS SDK

Build instructions for compiling Zigbee applications with CMake.

---

## Table of Contents

 - [CMake Build System](#cmake-build-system)
 - [Prerequisites](#prerequisites)
 - [Build Steps](#build-steps)
 - [Complete Build Workflow](#complete-build-workflow)
 - [Finding Compiled Binaries](#finding-compiled-binaries)
 - [Troubleshooting](#troubleshooting)

---

### CMake Build System

The build system consists of two main components:

- **`CMakeLists.txt`** : Main file that includes all examples
- **`examples/zigbee_build_app.cmake`** : Reusable module to create Zigbee executables with different roles

#### Main CMakeLists.txt Structure

The main file includes all example CMakeLists:

```cmake
cmake_minimum_required(VERSION 3.10.2)
project(examples)

include(${CMAKE_CURRENT_SOURCE_DIR}/examples/onoff_server/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_cluster/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/level_control_sample/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/manuf_specific_cmds_and_attrs/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/tc_swap_out/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/multi_ep/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/distributed/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/light_sample/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/gp_proxy/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/scenes/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/ota_upgrade_nxp/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/thermostat/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/simple_gw/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/r23_new_api/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/cli_nxp/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/dualpan_nxp/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/ias_zone_sensor/CMakeLists.txt)
```

Each example defines its own build targets.

#### zigbee_build_app.cmake Structure

This module makes it easy to create Zigbee executables for different device roles.

##### Supported Zigbee Roles

| Role | Constant | Library | Compilation Flags |
|------|----------|---------|-------------------|
| **Coordinator (ZC)** | `COORDINATOR` | `libzboss.a` | `-DZB_COORDINATOR_ROLE -DZB_ROUTER_ROLE` |
| **Router (ZR)** | `ROUTER` | `libzboss.a` | `-DZB_ROUTER_ROLE` |
| **End Device (ZED)** | `END_DEVICE` | `libzboss.ed.a` | `-DZB_ED_ROLE` |

##### Core Function: add_zigbee_executable()

The `add_zigbee_executable()` function simplifies building Zigbee applications:

```cmake
add_zigbee_executable(
   NAME <executable_name>
   ROLE <COORDINATOR|ROUTER|END_DEVICE>
   SOURCES <source_file1> <source_file2> ...
)
```

**Parameters:**
- `NAME` : Name of the executable to build
- `ROLE` : Zigbee device role (COORDINATOR, ROUTER, or END_DEVICE)
- `SOURCES` : List of source files to compile

**What it does automatically:**
- Sets appropriate compilation flags based on role
- Adds optimization flags (`-O3`)
- Strips symbols (`-s`)
- Includes ZBOSS headers
- Links with the correct ZBOSS library
- Links with readline library
- Sets output directory to `bin/examples/`
- Configures installation target

##### Example CMakeLists.txt for a Simple Application

```cmake
cmake_minimum_required(VERSION 3.10.2)

project(simple_gw)

# Include the Zigbee build system
include(${CMAKE_CURRENT_SOURCE_DIR}/../zigbee_build_app.cmake)

# Build simple_gw coordinator
add_zigbee_executable(
   NAME simple_gw
   ROLE COORDINATOR
   SOURCES 
       examples/simple_gw/simple_gw.c
       examples/simple_gw/ias_cie_addon.c
)
```

##### Example CMakeLists.txt for a Complex Application

For applications with multiple executables (like cli_nxp):

```cmake
cmake_minimum_required(VERSION 3.10.2)

project(cli_nxp)

# Include the Zigbee build system
include(${CMAKE_CURRENT_SOURCE_DIR}/../zigbee_build_app.cmake)

# Common sources for CLI applications
set(CLI_COMMON_SOURCES
   examples/cli_nxp/cli_main.c
   examples/cli_nxp/cli_config.c
   examples/cli_nxp/cli_network.c
   examples/cli_nxp/cli_endpoint.c
   examples/cli_nxp/cli_cluster.c
   examples/cli_nxp/cli_zdo.c
   examples/cli_nxp/cli_installcode.c
   examples/cli_nxp/cli_nvram.c
   examples/cli_nxp/cli_tools.c
)

# Console variant sources
set(CONSOLE_SOURCES ${CLI_COMMON_SOURCES} examples/cli_nxp/cli_menu_console.c)

# Socket variant sources
set(SOCKET_SOURCES ${CLI_COMMON_SOURCES} examples/cli_nxp/cli_menu_socket.c)

# Build all CLI executables
add_zigbee_executable(NAME cli_nxp_zczr ROLE COORDINATOR SOURCES ${CONSOLE_SOURCES})
add_zigbee_executable(NAME cli_nxp_zed ROLE END_DEVICE SOURCES ${CONSOLE_SOURCES})

add_zigbee_executable(NAME zb_daemon_zczr ROLE COORDINATOR SOURCES ${SOCKET_SOURCES})
add_zigbee_executable(NAME zb_daemon_zed ROLE END_DEVICE SOURCES ${SOCKET_SOURCES})

add_zigbee_executable(NAME zb-ctl ROLE COORDINATOR SOURCES examples/cli_nxp/zb-ctl.c)
```

##### Creating a New Application

To create a new Zigbee application `my_new_app`:

1. Create directory: `examples/my_new_app/`
2. Add your source files: `my_new_app_zc.c`, `my_new_app_zr.c`, etc.
3. Create `examples/my_new_app/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10.2)

project(my_new_app)

# Include the Zigbee build system
include(${CMAKE_CURRENT_SOURCE_DIR}/../zigbee_build_app.cmake)

# Build coordinator
add_zigbee_executable(
   NAME my_new_app_zc
   ROLE COORDINATOR
   SOURCES examples/my_new_app/my_new_app_zc.c
)

# Build router
add_zigbee_executable(
   NAME my_new_app_zr
   ROLE ROUTER
   SOURCES examples/my_new_app/my_new_app_zr.c
)

# Build end device
add_zigbee_executable(
   NAME my_new_app_zed
   ROLE END_DEVICE
   SOURCES examples/my_new_app/my_new_app_zed.c
)
```

4. Add to main `CMakeLists.txt`:

```cmake
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/my_new_app/CMakeLists.txt)
```

5. Build:

```bash
rm -rf build && cmake -B build && cmake --build build
```

# Build each target
```bash
cmake --build build --target on_off_output_zc
cmake --build build --target on_off_switch_zed
```

# Clean all and rebuild on_off_output_zc only
```bash
cmake --build build --target on_off_output_zc --clean-first
```
### Prerequisites

The `libzboss.a` libraries are compiled for **ARM64 (aarch64)**. You must source the Yocto SDK environment before building.

**Required step:**

```bash
source /opt/fsl-imx-xwayland/6.18.2-whinlatter-matter/environment-setup-armv8a-poky-linux
export CC=`echo ${CC} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CXX=`echo ${CXX} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CPP=`echo ${CPP} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
```

This will automatically set up:
- Cross-compiler (`$CC`, `$CXX`)
- Sysroot path
- Compiler flags
- Linker flags

**Verify the environment is correctly set:**

```bash
echo $CC
```

Expected output should show `aarch64-poky-linux-gcc` with appropriate flags.

**Dependencies:**
- CMake >= 3.10.2
- Yocto SDK for i.MX (sourced as above)
- libreadline (included in the SDK sysroot)

### Build Steps

#### 1. Source the Yocto SDK Environment

**This step is mandatory before any build operation:**

```bash
source /opt/fsl-imx-xwayland/6.18.2-whinlatter-matter/environment-setup-armv8a-poky-linux
export CC=`echo ${CC} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CXX=`echo ${CXX} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CPP=`echo ${CPP} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
```

#### 2. Clean Previous Build Files

```bash
rm -rf build/
```

#### 3. Configure CMake

```bash
cmake -B build -S .
```

#### 4. Build All Examples

```bash
cmake --build build
```

#### 5. Build with Parallel Jobs (Faster)

```bash
cmake --build build -j$(nproc)
```

#### 6. Build a Specific Target

```bash
cmake --build build --target simple_gw
```

```bash
cmake --build build --target cli_nxp_zczr
```

#### 7. Build in Verbose Mode

```bash
cmake --build build --verbose
```

#### 8. Install Binaries (Optional)

To install binaries to a custom location:

```bash
cmake --install build --prefix /path/to/install
```

### Complete Build Workflow

```bash
# Step 1: Source Yocto SDK
source /opt/fsl-imx-xwayland/6.18.2-whinlatter-matter/environment-setup-armv8a-poky-linux
export CC=`echo ${CC} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CXX=`echo ${CXX} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CPP=`echo ${CPP} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
```

```bash
# Step 2: Clean previous build
rm -rf build/
```

```bash
# Step 3: Configure
cmake -B build -S .
```

```bash
# Step 4: Build all targets
cmake --build build -j$(nproc)
```

### Finding Compiled Binaries

```bash
find ./build -type f -executable
```

### Troubleshooting

#### Error: "Relocations in generic ELF (EM: 183)"

**Cause**: You're using the wrong compiler (x86_64 instead of ARM)

**Solution**: Make sure you sourced the Yocto SDK environment

```bash
source /opt/fsl-imx-xwayland/6.18.2-whinlatter-matter/environment-setup-armv8a-poky-linux
export CC=`echo ${CC} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CXX=`echo ${CXX} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
export CPP=`echo ${CPP} | sed 's/-O2 -D_FORTIFY_SOURCE=2 //g'`
```

```bash
rm -rf build/
```

```bash
cmake -B build -S .
```

#### Error: "libreadline.so not found"

**Cause**: The readline library is not available in the ARM sysroot

**Solution**: Ensure your Yocto SDK includes libreadline or modify CMakeLists to exclude it

#### Verify Library Architecture

```bash
file libs/libzboss.a
```

Expected output: `libs/libzboss.a: current ar archive`

```bash
readelf -h libs/libzboss.a | grep Machine
```

Should display: `Machine: AArch64`

#### Verify Environment is Sourced

```bash
echo $CC
```

```bash
echo $SDKTARGETSYSROOT
```

Both should show ARM-related paths and configurations.

#### Build System Not Finding Sources

**Cause**: Incorrect path in SOURCES

**Solution**: All source paths must be relative to the repository root:

```cmake
# Correct
SOURCES examples/simple_gw/simple_gw.c

# Incorrect
SOURCES simple_gw.c
```

#### Clean Build

If you encounter persistent issues, perform a complete clean:

```bash
rm -rf build/ bin/ CMakeCache.txt CMakeFiles/
```

```bash
cmake -B build -S .
```

```bash
cmake --build build
```

---

[← Back to Main README](README.md)
