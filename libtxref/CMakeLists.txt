
# Define library target

set(LIB_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/libtxref/libtxref.h
)

set(LIB_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/txref.cpp
)

add_library(txref STATIC ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES})
add_library(txref::txref ALIAS txref)

set_property(
        TARGET txref
        PROPERTY PUBLIC_HEADER "${LIB_HEADER_FILES}"
)

# Set include directory

target_include_directories(txref
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/libtxref>
)

# Misc properties

target_compile_features(txref PRIVATE cxx_std_11)
set_target_properties(txref PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(txref PUBLIC bech32::bech32)

# Set version

target_compile_definitions(txref PRIVATE -DLIBTXREF_VERSION_MAJOR=${LIBTXREF_VERSION_MAJOR})
target_compile_definitions(txref PRIVATE -DLIBTXREF_VERSION_MINOR=${LIBTXREF_VERSION_MINOR})
target_compile_definitions(txref PRIVATE -DLIBTXREF_VERSION_PATCH=${LIBTXREF_VERSION_PATCH})


if(INSTALL_LIBTXREF)

    # Configuration

    set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
    set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
    set(namespace "txref::")

    # Include module with function 'write_basic_package_version_file' and
    # 'configure_package_config_file'
    include(CMakePackageConfigHelpers)

    # Configure '<PROJECT-NAME>ConfigVersion.cmake'
    # Note: PROJECT_VERSION is used as a VERSION
    write_basic_package_version_file(
            "${version_config}"
            COMPATIBILITY SameMajorVersion
    )

    # Install '<PROJECT-NAME>ConfigVersion.cmake'
    install(
            FILES ${version_config}
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/libtxref
    )

    # Configure '<PROJECT-NAME>Config.cmake' from .in file
    configure_package_config_file(
            "${PROJECT_SOURCE_DIR}/cmake/libtxrefConfig.cmake.in"
            ${project_config}
            INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/libtxref
    )

    # Install '<PROJECT-NAME>Config.cmake'
    install(FILES ${project_config}
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/libtxref
    )

    # Install Targets

    install(
            EXPORT txref_Targets
            FILE ${PROJECT_NAME}Targets.cmake
            NAMESPACE ${namespace}
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/libtxref
    )

    install(
            TARGETS txref
            EXPORT txref_Targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libtxref
    )


endif()
