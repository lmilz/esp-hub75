add_library(hub75
	src/hub75.c
)

target_include_directories(hub75 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
