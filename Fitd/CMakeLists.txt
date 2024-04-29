include_directories(
    "."
    "${CMAKE_SOURCE_DIR}/FitdLib"
)

file(GLOB FITD_DATA
    "${CMAKE_SOURCE_DIR}/data/*"
)

file(GLOB SRC_ROOT
    "*.cpp"
    "*.h"
)

set(SOURCES
 ${SOURCES}
 ${SRC_ROOT}
)

IF(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    set(SOURCES
     ${SOURCES}
     "bgfxPatch.mm"
    )
endif()

source_group("src" FILES ${SRC_ROOT})

add_executable (Fitd ${SOURCES})

TARGET_LINK_LIBRARIES(Fitd FitdLib)
