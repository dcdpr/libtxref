
add_executable(UnitTests_txref main.cpp test_Txref.cpp test_Txref_api.cpp)

target_compile_features(UnitTests_txref PRIVATE cxx_std_11)
set_target_properties(UnitTests_txref PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(UnitTests_txref
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/libtxref)

target_link_libraries(UnitTests_txref PUBLIC txref gtest gmock rapidcheck_gtest)

add_test(NAME UnitTests_txref
         COMMAND UnitTests_txref)


add_executable(txref_c_api_tests
        txref_c_api_tests.c
        )

target_compile_features(txref_c_api_tests PRIVATE c_std_99)
set_target_properties(txref_c_api_tests PROPERTIES C_EXTENSIONS OFF)

target_link_libraries(txref_c_api_tests PUBLIC txref)

add_test(NAME UnitTests_C_api_txref
        COMMAND txref_c_api_tests)
