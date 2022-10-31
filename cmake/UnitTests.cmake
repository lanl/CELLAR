function(output_eap_core_add_mpi_test_usage)
    message(FATAL_ERROR "Error: eap_core_add_mpi_test USAGE:
        eap_core_add_mpi_test(
            NAME <TEST_NAME>
            COMMAND <COMMAND>...)
    ")
endfunction()

function(eap_core_add_mpi_test)
    if (ARGC LESS 4)
        output_eap_core_add_mpi_test_usage()
    endif()

    if (NOT ARGV0 STREQUAL "NAME")
        output_eap_core_add_mpi_test_usage()
    endif()

    set(TEST_NAME ${ARGV1})

    if (NOT ARGV2 STREQUAL "COMMAND")
        output_eap_core_add_mpi_test_usage()
    endif()

    set(COMMAND_NAME ${ARGV3})

    set(CURRENT_ARG 4)
    while(NOT CURRENT_ARG EQUAL ARGC)
        list(APPEND COMMAND_ARGS "${ARGV${CURRENT_ARG}}")
        math(EXPR CURRENT_ARG "${CURRENT_ARG} + 1")
    endwhile()

    set(NUM_RANKS 4)
    if (KOKKOS_INTERNAL_USE_OPENMP)
        set(NUM_THREADS 4)
        list(APPEND ENVIRONMENT OMP_NUM_THREADS=4)
        math(EXPR NUM_PROCESSORS "${NUM_RANKS} * ${NUM_THREADS}")
    else()
        set(NUM_PROCESSORS ${NUM_RANKS})
    endif()

    if (KOKKOS_INTERNAL_USE_CUDA)
        list(APPEND ENVIRONMENT CUDA_LAUNCH_BLOCKING=1 CUDA_MANAGED_FORCE_DEVICE_ALLOC=1)
    endif()

    add_test(
        NAME ${TEST_NAME}
        COMMAND
            ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${NUM_RANKS} ${MPIEXEC_PREFLAGS}
                ${COMMAND_NAME} ${MPIEXEC_POSTFLAGS} ${COMMAND_ARGS}
    )

    set_tests_properties(
        ${TEST_NAME}
        PROPERTIES
            ENVIRONMENT "${ENVIRONMENT}")
    set_tests_properties(
        ${TEST_NAME}
        PROPERTIES PROCESSORS ${NUM_PROCESSORS}
    )
endfunction()

function(output_eap_core_add_serial_test_usage)
    message(FATAL_ERROR "Error: eap_core_add_serial_test USAGE:
        eap_core_add_serial_test(
            NAME <TEST_NAME>
            COMMAND <COMMAND>...)
    ")
endfunction()

function(eap_core_add_serial_test)
    if (ARGC LESS 4)
        output_eap_core_add_serial_test_usage()
    endif()

    if (NOT ARGV0 STREQUAL "NAME")
        output_eap_core_add_serial_test_usage()
    endif()

    set(TEST_NAME ${ARGV1})

    if (NOT ARGV2 STREQUAL "COMMAND")
        output_eap_core_add_serial_test_usage()
    endif()

    set(COMMAND_NAME ${ARGV3})

    set(CURRENT_ARG 4)
    while(NOT CURRENT_ARG EQUAL ARGC)
        list(APPEND COMMAND_ARGS "${ARGV${CURRENT_ARG}}")
        math(EXPR CURRENT_ARG "${CURRENT_ARG} + 1")
    endwhile()

    set(NUM_RANKS 1)
    if (KOKKOS_INTERNAL_USE_OPENMP)
        set(NUM_THREADS 4)
        list(APPEND ENVIRONMENT OMP_NUM_THREADS=4)
        math(EXPR NUM_PROCESSORS "${NUM_RANKS} * ${NUM_THREADS}")
    else()
        set(NUM_PROCESSORS ${NUM_RANKS})
    endif()

    if (KOKKOS_INTERNAL_USE_CUDA)
        list(APPEND ENVIRONMENT CUDA_LAUNCH_BLOCKING=1 CUDA_MANAGED_FORCE_DEVICE_ALLOC=1)
    endif()

    add_test(
        NAME ${TEST_NAME}
        COMMAND
            ${COMMAND_NAME} ${COMMAND_ARGS}
    )

    set_tests_properties(
        ${TEST_NAME}
        PROPERTIES
            ENVIRONMENT "${ENVIRONMENT}")
    set_tests_properties(
        ${TEST_NAME}
        PROPERTIES PROCESSORS ${NUM_PROCESSORS}
    )
endfunction()