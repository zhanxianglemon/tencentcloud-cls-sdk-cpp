cmake_minimum_required(VERSION 3.15)
set(VCPKG_TARGET_TRIPLET "x64-windows")
set(CMAKE_TOOL_CHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
project(tencent-log-sdk-cpp-windows)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wno-template-body)
endif()

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(Protobuf REQUIRED)
find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)


include_directories(./)
include_directories(./x64-windows/include)

#set(PROTO_FILE
#        "${CMAKE_SOURCE_DIR}/cls_logs.proto"
#        "${CMAKE_SOURCE_DIR}/logproducerconfig.proto")
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
#    ${PROTO_FILE})


aux_source_directory(cls src)
aux_source_directory(lz4 src)
aux_source_directory(rapidjson src)

add_executable(cls ${src} cls_logs.pb.cc logproducerconfig.pb.cc)

target_link_libraries(cls PRIVATE protobuf::libprotobuf)
target_link_libraries(cls PRIVATE CURL::libcurl)
target_link_libraries(cls PRIVATE Boost::boost)
target_link_libraries(cls PRIVATE Boost::system)
target_link_libraries(cls PRIVATE Boost::thread)
target_link_libraries(cls PRIVATE OpenSSL::SSL)
target_link_libraries(cls PRIVATE OpenSSL::Crypto)

if(MSVC)
    set_target_properties(cls PROPERTIES COMPILE_FLAGS "/EHsc") 
else()
    set_target_properties(cls PROPERTIES COMPILE_FLAGS "-fexceptions")  
endif()

target_compile_definitions(cls PRIVATE
        BOOST_THROW_EXCEPTION
        BOOST_SYSTEM_NO_DEPRECATED
)