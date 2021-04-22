add_executable(txref_cpp_example cpp_example.cpp)

target_compile_features(txref_cpp_example PRIVATE cxx_std_11)
target_compile_options(txref_cpp_example PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(txref_cpp_example PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(txref_cpp_example bech32 txref)

#

add_executable(txref_cpp_usage_example cpp_usage_example.cpp)

target_compile_features(txref_cpp_usage_example PRIVATE cxx_std_11)
target_compile_options(txref_cpp_usage_example PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(txref_cpp_usage_example PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(txref_cpp_usage_example bech32 txref)

#

add_executable(txref_c_example c_example.c)

target_compile_features(txref_c_example PRIVATE c_std_99)
set_target_properties(txref_c_example PROPERTIES C_EXTENSIONS OFF)

target_link_libraries(txref_c_example bech32 txref)

#

add_executable(txref_c_usage_example c_usage_example.c)

target_compile_features(txref_c_usage_example PRIVATE c_std_99)
set_target_properties(txref_c_usage_example PROPERTIES C_EXTENSIONS OFF)

target_link_libraries(txref_c_usage_example bech32 txref)

#

add_executable(txrefEncode txrefEncode.cpp)

target_compile_features(txrefEncode PRIVATE cxx_std_11)
target_compile_options(txrefEncode PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(txrefEncode PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(txrefEncode bech32 txref)

#

add_executable(txrefDecode txrefDecode.cpp)

target_compile_features(txrefDecode PRIVATE cxx_std_11)
target_compile_options(txrefDecode PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(txrefDecode PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(txrefDecode bech32 txref)
