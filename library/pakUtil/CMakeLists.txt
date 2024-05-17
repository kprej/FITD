set(INC_DIR ${PROJECT_SOURCE_DIR}/include/pakUtil)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/library/pakUtil)

set(INC
    ${INC_DIR}/buffer.h
    ${INC_DIR}/pakFile.h
    )

set(SRC
    ${SRC_DIR}/pakFile.cpp
    ${SRC_DIR}/unpack.cpp
    ${SRC_DIR}/unpack.h
    )

add_library (pakUtil STATIC ${INC} ${SRC})

target_include_directories(pakUtil PUBLIC ${INC_DIR})
target_link_libraries(pakUtil plog zlib)
