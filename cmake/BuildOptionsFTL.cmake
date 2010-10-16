ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)
SET(CMAKE_EXE_LINKER_FLAGS "-ldl ${CMAKE_EXE_LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "-ldl ${CMAKE_SHARED_LINKER_FLAGS}")

IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	ADD_DEFINITIONS(-D_POSIX_PTHREAD_SEMANTICS)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	# Darwin 10.0: Snow (aka 10.6)
	# Darwin 9.x: Leopard (aka 10.5)
	# Darwin 8.x: Tiger (aka 10.4)
	ADD_DEFINITIONS(-pthread) #  -D_REENTRANT
	# LINK_LIBRARIES(pthread)
	SET(CMAKE_SHARED_LINKER_FLAGS "-pthread ${CMAKE_SHARED_LINKER_FLAGS}")
	SET(CMAKE_EXE_LINKER_FLAGS "-pthread ${CMAKE_EXE_LINKER_FLAGS}")
	SET(CMAKE_SHARED_LINKER_FLAGS "-U_undefined -undefined dynamic_lookup ${CMAKE_SHARED_LINKER_FLAGS}") # to make "_environ" available
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	ADD_DEFINITIONS(-D_XOPEN_SOURCE=600)
	SET(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
	LINK_LIBRARIES(rt util)
ELSE(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
	# others, like OpenBSD
	# ADD_DEFINITIONS(-D_REENTRANT)
	SET(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
	LINK_LIBRARIES(util)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

SET(CMAKE_CXX_FLAGS_DEBUG "-Wall ${CMAKE_CXX_FLAGS_DEBUG}")

# MESSAGE("CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
# MESSAGE("CMAKE_HOST_SYSTEM_PROCESSOR = ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# ADD_DEFINITIONS(-std=c++98)
