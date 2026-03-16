cmake_minimum_required(VERSION 3.15)
project(ParserModule)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_TOOLCHAIN_FILE "build/dependencies/conan_toolchain.cmake")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/dependencies")

set(SOURCES
    src/main.cpp
    src/PacketParser.cpp
    src/NATSJetstream.cpp
    src/ParserConfig.cpp
    src/PriorityQueue.cpp
    src/PacketDeserializer.cpp
    src/ParsedDataSerializer.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(main ${SOURCES})

find_package(inih REQUIRED)
find_package(libpcap REQUIRED)
find_package(cnats REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(main 
    inih::inih
    libpcap::libpcap 
    cnats::nats_static 
    spdlog::spdlog 
    Logger         # This will link libLogger.a
)