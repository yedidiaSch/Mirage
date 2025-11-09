# FindRtMidi.cmake - Find RtMidi library
# This will define:
#  RTMIDI_FOUND - system has RtMidi
#  RTMIDI_INCLUDE_DIRS - the RtMidi include directories
#  RTMIDI_LIBRARIES - link these to use RtMidi

find_path(RTMIDI_INCLUDE_DIR
    NAMES RtMidi.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /usr/include/rtmidi
    DOC "RtMidi include directory"
)

find_library(RTMIDI_LIBRARY
    NAMES rtmidi RtMidi
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /usr/lib/x86_64-linux-gnu
    DOC "RtMidi library"
)

if(RTMIDI_INCLUDE_DIR AND RTMIDI_LIBRARY)
    set(RTMIDI_FOUND TRUE)
    set(RTMIDI_INCLUDE_DIRS ${RTMIDI_INCLUDE_DIR})
    set(RTMIDI_LIBRARIES ${RTMIDI_LIBRARY})
endif()

mark_as_advanced(
    RTMIDI_INCLUDE_DIR
    RTMIDI_LIBRARY
)