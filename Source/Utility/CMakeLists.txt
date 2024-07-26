list(APPEND LOCAL_SOURCE_FILES
        Corvus.h
        Log.h
        Timer.h
)

foreach(file ${LOCAL_SOURCE_FILES})
    list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()
set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
