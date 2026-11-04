include(CheckCXXCompilerFlag)

function(cc_enable_sanitizers target)
    if(NOT CC_ENABLE_SANITIZERS)
        return()
    endif()

    if(MSVC)
        message(WARNING "CC_ENABLE_SANITIZERS is not configured for MSVC; use a supported Clang or GCC toolchain.")
        return()
    endif()

    check_cxx_compiler_flag("-fsanitize=address,undefined" CC_HAS_ASAN_UBSAN)
    if(NOT CC_HAS_ASAN_UBSAN)
        message(FATAL_ERROR "The selected compiler does not support -fsanitize=address,undefined")
    endif()

    target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(${target} PRIVATE -fsanitize=address,undefined)
endfunction()
