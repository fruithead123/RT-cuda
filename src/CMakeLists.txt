# 1. Gather all source files
file(GLOB_RECURSE CORE_SOURCES "*.cpp" "*.cu")

# 2. Locate all packages FIRST so variables are fully populated
find_package(CUDAToolkit REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED) # Provides modern OpenGL targets safely

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# 3. Create the library target
add_library(pt_core STATIC ${CORE_SOURCES})

# 4. Include paths (Both your internal headers and GLFW)
target_include_directories(pt_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    ${GLFW_INCLUDE_DIRS}
)

# 5. Provide explicit library directories for PkgConfig
target_link_directories(pt_core PUBLIC 
    ${GLFW_LIBRARY_DIRS}
)

# 6. Link libraries using safe, explicit targets
target_link_libraries(pt_core PUBLIC 
    CUDA::cudart 
    GLEW::GLEW 
    OpenGL::GL            # Replaced raw 'GL' with a robust, platform-agnostic target
    ${GLFW_LIBRARIES}     # Resolves to 'glfw' but now backed by target_link_directories
)

# 7. Enable CUDA Separable Compilation
set_target_properties(pt_core PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)

# 8. Optimization flags
target_compile_options(pt_core PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-O3 -march=native>
    $<$<COMPILE_LANGUAGE:CUDA>:--use_fast_math>
)