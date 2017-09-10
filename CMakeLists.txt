cmake_minimum_required(VERSION 3.0)

project(Polymost)

include_directories(${PROJECT_BINARY_DIR})

ADD_LIBRARY(Polymost SHARED PolyMost.cpp)

# define the project
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



# Find the POCO paths and libs
if(NOT DEFINED POCO)
	set(POCO CACHE STRING "POCO")
endif()
if(DEFINED POCO)
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${POCO})
	message("CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})
endif()

# Un-comment for static compiling
#add_definitions( -DPOCO_STATIC ) 

find_package(Poco REQUIRED Foundation Util Net Data DataSQLite XML JSON NetSSL)

# link the app against POCO
target_link_libraries(Polymost Poco::Foundation Poco::Util Poco::Net Poco::Data Poco::DataSQLite Poco::XML Poco::JSON Poco::NetSSL)

if(WIN32)
	SET(DLLS PocoFoundation PocoUtil PocoNet PocoData PocoDataSQLite PocoXML PocoJSON PocoNetSSL)
	foreach(DLL ${DLLS})
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			SET(DLL ${DLL}d)
		endif()
		add_custom_command(TARGET Polymost POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_PREFIX_PATH}/bin/${DLL}.dll"
			$<TARGET_FILE_DIR:Polymost>)
	endforeach(DLL)
endif()

#add_library(PolyChat MODULE "../PolyChat/include/IProtocolPlugin.h
# Find the POCO paths and libs
if(NOT DEFINED POLYCHAT_INCLUDE)
	set(POLYCHAT_INCLUDE CACHE STRING "POLYCHAT_INCLUDE")
endif()
if(DEFINED POLYCHAT_INCLUDE)
	target_include_directories(Polymost PUBLIC ${POLYCHAT_INCLUDE})
endif()

