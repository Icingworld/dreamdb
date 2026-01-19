# ============================================================================
# Protobuf Generate C++
# ============================================================================

function(dreamdb_protobuf_generate_cpp OUT_SRCS OUT_HDRS)
  set(options)
  set(oneValueArgs PROTO_ROOT OUT_DIR)
  set(multiValueArgs PROTOS)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT ARG_PROTO_ROOT)
    message(FATAL_ERROR "dreamdb_protobuf_generate_cpp: PROTO_ROOT is required")
  endif()
  if (NOT ARG_OUT_DIR)
    message(FATAL_ERROR "dreamdb_protobuf_generate_cpp: OUT_DIR is required")
  endif()
  if (NOT ARG_PROTOS)
    message(FATAL_ERROR "dreamdb_protobuf_generate_cpp: PROTOS is required")
  endif()

  # 规范化路径（转换为绝对路径）
  get_filename_component(ARG_PROTO_ROOT "${ARG_PROTO_ROOT}" ABSOLUTE)
  get_filename_component(ARG_OUT_DIR "${ARG_OUT_DIR}" ABSOLUTE)

  # 统一路径分隔符为正斜杠（protoc 在 Windows 上也支持正斜杠）
  string(REPLACE "\\" "/" ARG_PROTO_ROOT "${ARG_PROTO_ROOT}")
  string(REPLACE "\\" "/" ARG_OUT_DIR "${ARG_OUT_DIR}")

  # 检查 PROTO_ROOT 目录是否存在
  if(NOT EXISTS "${ARG_PROTO_ROOT}")
    message(FATAL_ERROR "PROTO_ROOT directory does not exist: ${ARG_PROTO_ROOT}")
  endif()
  if(NOT IS_DIRECTORY "${ARG_PROTO_ROOT}")
    message(FATAL_ERROR "PROTO_ROOT is not a directory: ${ARG_PROTO_ROOT}")
  endif()

  # 生成输出列表
  set(_srclist "")
  set(_hdrlist "")
  set(_relprotos "")

  foreach(proto ${ARG_PROTOS})
    # 规范化 proto 文件路径
    get_filename_component(_abs_proto "${proto}" ABSOLUTE)
    string(REPLACE "\\" "/" _abs_proto "${_abs_proto}")

    # 检查文件是否存在
    if(NOT EXISTS "${_abs_proto}")
      message(FATAL_ERROR "Proto file not found: ${_abs_proto}")
    endif()

    # 计算相对于 PROTO_ROOT 的相对路径
    file(RELATIVE_PATH _relproto "${ARG_PROTO_ROOT}" "${_abs_proto}")

    # 检查相对路径是否有效（如果返回空或包含 ..，说明路径有问题）
    if("${_relproto}" STREQUAL "" OR "${_relproto}" MATCHES "^\\.\\.")
      message(FATAL_ERROR "Could not make proto path relative: ${proto} (PROTO_ROOT: ${ARG_PROTO_ROOT}, absolute: ${_abs_proto})")
    endif()

    get_filename_component(_relproto_dir "${_relproto}" DIRECTORY)
    get_filename_component(_name_we "${_abs_proto}" NAME_WE)

    # 构建输出路径，保持目录结构（使用正斜杠）
    if(_relproto_dir)
      list(APPEND _srclist "${ARG_OUT_DIR}/${_relproto_dir}/${_name_we}.pb.cc")
      list(APPEND _hdrlist "${ARG_OUT_DIR}/${_relproto_dir}/${_name_we}.pb.h")
      # 确保输出目录存在
      file(MAKE_DIRECTORY "${ARG_OUT_DIR}/${_relproto_dir}")
    else()
      list(APPEND _srclist "${ARG_OUT_DIR}/${_name_we}.pb.cc")
      list(APPEND _hdrlist "${ARG_OUT_DIR}/${_name_we}.pb.h")
    endif()

    # 添加相对路径到 protoc 参数列表（使用正斜杠）
    list(APPEND _relprotos "${_relproto}")
  endforeach()

  # 确保输出目录存在
  file(MAKE_DIRECTORY "${ARG_OUT_DIR}")

  # 在 Windows 上，protoc 可能需要原生路径格式
  if(WIN32)
    file(TO_NATIVE_PATH "${ARG_PROTO_ROOT}" _native_proto_root)
    file(TO_NATIVE_PATH "${ARG_OUT_DIR}" _native_out_dir)
  else()
    set(_native_proto_root "${ARG_PROTO_ROOT}")
    set(_native_out_dir "${ARG_OUT_DIR}")
  endif()

  add_custom_command(
    OUTPUT ${_srclist} ${_hdrlist}
    COMMAND $<TARGET_FILE:dreamdb::protoc>
            --proto_path=${_native_proto_root}
            --cpp_out=${_native_out_dir}
            ${_relprotos}
    DEPENDS dreamdb::protoc ${ARG_PROTOS}
    COMMENT "Generating protobuf C++ sources from ${_relprotos}"
    VERBATIM
  )

  set(${OUT_SRCS} ${_srclist} PARENT_SCOPE)
  set(${OUT_HDRS} ${_hdrlist} PARENT_SCOPE)
endfunction()
