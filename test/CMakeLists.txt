cmake_minimum_required(VERSION 3.15)
set(VCPKG_TARGET_TRIPLET "x64-windows")
set(CMAKE_TOOL_CHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
project(tencent-log-sdk-cpp-windows)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wno-template-body)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)


include_directories(../)
include_directories(../x64-windows/include)

#set(PROTO_FILE
#        "${CMAKE_SOURCE_DIR}/cls_logs.proto"
#        "${CMAKE_SOURCE_DIR}/logproducerconfig.proto")
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
#    ${PROTO_FILE})


aux_source_directory(cls src)
aux_source_directory(lz4 src)
aux_source_directory(rapidjson src)

add_executable(tencent-log-sdk-cpp-windows ${src} cls_logs.pb.cc logproducerconfig.pb.cc "test/tencentclssdk_test.cpp" "test/tencentclssdk_benchmark.cpp")

target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE protobuf::libprotobuf)
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE CURL::libcurl)
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE Boost::boost)
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE Boost::system)
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE Boost::thread)
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE OpenSSL::SSL)
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE OpenSSL::Crypto)
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE GTest::gtest )
target_link_libraries(tencent-log-sdk-cpp-windows PRIVATE benchmark::benchmark )


if(MSVC)
    set_target_properties(tencent-log-sdk-cpp-windows PROPERTIES COMPILE_FLAGS "/EHsc")  
else()
    set_target_properties(tencent-log-sdk-cpp-windows PROPERTIES COMPILE_FLAGS "-fexceptions") 
endif()

target_compile_definitions(tencent-log-sdk-cpp-windows PRIVATE
        BOOST_THROW_EXCEPTION
        BOOST_SYSTEM_NO_DEPRECATED
)