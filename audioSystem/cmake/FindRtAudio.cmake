# FindRtAudio.cmake - Find RtAudio library
# This will define:
#  RTAUDIO_FOUND - system has RtAudio
#  RTAUDIO_INCLUDE_DIRS - the RtAudio include directories
#  RTAUDIO_LIBRARIES - link these to use RtAudio

find_path(RTAUDIO_INCLUDE_DIR
    NAMES RtAudio.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /usr/include/rtaudio
    DOC "RtAudio include directory"
)

find_library(RTAUDIO_LIBRARY
    NAMES rtaudio RtAudio
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /usr/lib/x86_64-linux-gnu
    DOC "RtAudio library"
)

if(RTAUDIO_INCLUDE_DIR AND RTAUDIO_LIBRARY)
    set(RTAUDIO_FOUND TRUE)
    set(RTAUDIO_INCLUDE_DIRS ${RTAUDIO_INCLUDE_DIR})
    set(RTAUDIO_LIBRARIES ${RTAUDIO_LIBRARY})
endif()

mark_as_advanced(
    RTAUDIO_INCLUDE_DIR
    RTAUDIO_LIBRARY
)