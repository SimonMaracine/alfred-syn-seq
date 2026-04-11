function(set_target_cpp_mode target)
    target_compile_features(${target} PRIVATE cxx_std_23)
    set_target_properties(
        ${target} PROPERTIES
        CXX_EXTENSIONS OFF
        CMAKE_CXX_STANDARD_REQUIRED ON
    )
endfunction()

function(set_target_warnings target)
    if(UNIX)
        target_compile_options(${target} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
    elseif(WIN32)
        target_compile_options(${target} PRIVATE "/W4")
        target_compile_definitions(${target} PRIVATE "_CRT_SECURE_NO_WARNINGS")
    endif()
endfunction()

function(set_target_platform_macros target)
    if(UNIX)
        target_compile_definitions(${target} PRIVATE "ALFRED_LINUX")
    elseif(WIN32)
        target_compile_definitions(${target} PRIVATE "ALFRED_WINDOWS")
    endif()
endfunction()

function(enable_target_diagnostics target)
    if(UNIX AND ALFRED_ASAN)
        target_compile_options(${target} PRIVATE "-g")

        if(ALFRED_ASAN_THREAD)
            target_compile_options(${target} PRIVATE "-fsanitize=thread" "-fno-omit-frame-pointer")
            target_link_options(${target} PRIVATE "-fsanitize=thread")
        else()
            target_compile_options(${target} PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer")
            target_link_options(${target} PRIVATE "-fsanitize=address" "-fsanitize=undefined")
        endif()
    endif()
endfunction()

function(enable_target_ipo target)
    if(ALFRED_IPO_SUPPORTED AND NOT ALFRED_ASAN)
        set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endfunction()
