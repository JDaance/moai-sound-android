cmake_minimum_required ( VERSION 2.8.5 )
project ( moai-sound-android )

set ( PLUGIN_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )

set ( MOAI_SOUND_ANDROID_INCLUDES 
    ${PLUGIN_ROOT}/
)

# Use all cpp files in directory as source code
file ( GLOB_RECURSE MOAI_SOUND_ANDROID_SRC "${PLUGIN_ROOT}/*.cpp" )
file ( GLOB_RECURSE MOAI_SOUND_ANDROID_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )

add_library ( moai-sound-android STATIC ${MOAI_SOUND_ANDROID_SRC} ${MOAI_SOUND_ANDROID_HEADERS} )
target_include_directories( moai-sound-android PUBLIC ${MOAI_SOUND_ANDROID_INCLUDES})
target_link_libraries (moai-sound-android moai-core)