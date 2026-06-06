add_executable(pathtracer pathtracer.cpp)
target_link_libraries(pathtracer PRIVATE 
    pt_core
)

set_target_properties(pathtracer PROPERTIES
    CUDA_RESOLVE_DEVICE_SYMBOLS ON 
    CUDA_ARCHITECTURES "89"
)