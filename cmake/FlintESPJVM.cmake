set(FLINTESPJVM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../FlintESPJVM" CACHE PATH "Path to the FlintESPJVM repository")

if(EXISTS "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Inc/flint.h")
    set(FLINTOS_HAS_FLINTESPJVM ON)

    set(FLINTESPJVM_SRCS
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_utf8.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_hook.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_mutex.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_common.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_const_pool.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_fields_data.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_field_info.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_method_info.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_java_class.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_java_object.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_java_string.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_java_thread.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_java_throwable.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_array_object.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_class_loader.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_execution.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_debugger.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_utf8_dict_node.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_file_reader.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_zip_file_reader.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_java_class_dict_node.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_java_string_dict_node.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Src/flint_native_interface.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Impl/Src/fatfs_system_io_api.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Impl/Src/lwip_system_net_api.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Impl/Src/freertos_system_thread_api.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_math.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_class.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_array.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_float.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_double.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_object.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_string.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_system.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_thread.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_method.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_common.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_io_file.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_character.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_reflection.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_constructor.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_io_file_descriptor.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_io_file_input_stream.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_io_file_output_stream.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_flint_socket_impl.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_flint_inet_address_impl.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_flint_socket_input_stream.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_flint_socket_output_stream.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Src/flint_native_flint_datagram_socket_impl.cpp"
        "${FLINTESPJVM_ROOT}/ESPJVM/SystemAPI/Src/esp_system_api.cpp"
    )

    set(FLINTESPJVM_INCS
        "${FLINTESPJVM_ROOT}/ESPJVM/User/Inc"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/VM/Inc"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Impl/Inc"
        "${FLINTESPJVM_ROOT}/ESPJVM/FlintJVM/Native/Inc"
        "${FLINTESPJVM_ROOT}/ESPJVM/SystemAPI/Inc"
    )
else()
    set(FLINTOS_HAS_FLINTESPJVM OFF)
    set(FLINTESPJVM_SRCS)
    set(FLINTESPJVM_INCS)
endif()
