function(set_target_cpp_mode TARGET)
    target_compile_features(${TARGET} PRIVATE cxx_std_23)
    set_target_properties(
        ${TARGET} PROPERTIES
        CXX_EXTENSIONS OFF
        CMAKE_CXX_STANDARD_REQUIRED ON
    )
endfunction()

function(enable_target_warnings TARGET)
    if(UNIX)
        target_compile_options(${TARGET} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
    elseif(WIN32)
        target_compile_options(${TARGET} PRIVATE "/W4")
    endif()
endfunction()

function(enable_target_diagnostics TARGET)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX)
        target_compile_options(${TARGET} PRIVATE "-g")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND ALFRED_ASAN)
        target_compile_options(${TARGET} PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer")
        target_link_options(${TARGET} PRIVATE "-fsanitize=address" "-fsanitize=undefined")
    endif()
endfunction()
