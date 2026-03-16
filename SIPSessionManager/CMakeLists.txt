cmake_minimum_required(VERSION 3.15)
project(ParserModule)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_TOOLCHAIN_FILE "build/dependencies/conan_toolchain.cmake")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/dependencies")

set(SOURCES
    src/main.cpp
    src/SessionManager.cpp
    src/SessionFSM.cpp
    src/NATSJetstream.cpp
    src/SessionManagerConfig.cpp
    src/ParsedPacketDeserializer.cpp
    src/SessionSerializer.cpp
    src/states/IdleState.cpp
    src/states/AcceptedState.cpp
    src/states/AwaitingAuthenticationState.cpp
    src/states/CallEstablishedState.cpp
    src/states/CancelledState.cpp
    src/states/CancellingState.cpp
    src/states/ClosingState.cpp
    src/states/FailedState.cpp
    src/states/InvitedState.cpp
    src/states/ProgressState.cpp
    src/states/SessionModificationRefusedState.cpp
    src/states/SessionModifiedState.cpp
    src/states/SessionModifyingState.cpp
    src/states/TerminatedState.cpp
    src/states/ForcedTerminatedState.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/states)

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
