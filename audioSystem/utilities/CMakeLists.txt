# Utilities core library sources
set(UTILITIES_CORE_SOURCES
    QueueThread.cpp
    TimerFd.cpp
    subject.cpp
    threadBase.cpp
)

# Create object library for utilities core (excluding test files)
add_library(utilities_core OBJECT ${UTILITIES_CORE_SOURCES})

target_include_directories(utilities_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/utilities
)

# Optional: Build utilities tests (separate executable)
option(BUILD_UTILITIES_TESTS "Build utilities test executable" OFF)

if(BUILD_UTILITIES_TESTS)
    add_executable(test_utilities test_utilities.cpp)
    target_link_libraries(test_utilities
        $<TARGET_OBJECTS:utilities_core>
        Threads::Threads
    )
endif()