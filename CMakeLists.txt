cmake_minimum_required(VERSION 3.0)

project(PolyMost)
project(PolyMostStatic)

set(OUTPUT_DIR ${CMAKE_HOME_DIRECTORY}/target/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})

set(excutable_filename ${CMAKE_SHARED_LIBRARY_PREFIX}PolyMost${CMAKE_SHARED_LIBRARY_SUFFIX})
set(xml_file ${OUTPUT_DIR}/plugin.xml)


include_directories(${PROJECT_BINARY_DIR})

set(src src/PolyMost.cpp src/ConnectCommand.cpp)
ADD_LIBRARY(PolyMost SHARED ${src})
ADD_LIBRARY(PolyMostStatic STATIC ${src})
set_target_properties(PolyMost PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(PolyMostStatic PROPERTIES LINKER_LANGUAGE CXX)

# define the project
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")


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
target_link_libraries(PolyMost Poco::Foundation Poco::Util Poco::Net Poco::Data Poco::DataSQLite Poco::XML Poco::JSON Poco::NetSSL Poco::Crypto)
target_link_libraries(PolyMostStatic Poco::Foundation Poco::Util Poco::Net Poco::Data Poco::DataSQLite Poco::XML Poco::JSON Poco::NetSSL Poco::Crypto)

if(false AND WIN32)
	SET(DLLS PocoFoundation PocoUtil PocoNet PocoData PocoDataSQLite PocoXML PocoJSON PocoNetSSL PocoCrypto)
	foreach(DLL ${DLLS})
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			SET(DLL ${DLL}d)
		endif()
		add_custom_command(TARGET PolyMost POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_PREFIX_PATH}/bin/${DLL}.dll"
			$<TARGET_FILE_DIR:PolyMost>)
	endforeach(DLL)
endif()

# Find the POCO paths and libs
if(NOT DEFINED POLYCHAT)
	set(POLYCHAT CACHE STRING "POLYCHAT")
endif()
if(DEFINED POLYCHAT)
	#target_include_directories(Polymost PUBLIC ${POLYCHAT_INCLUDE})
	include_directories(${POLYCHAT})
	target_link_libraries(PolyMost ${POLYCHAT}/target/${CMAKE_STATIC_LIBRARY_PREFIX}PolyChat${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

#find_package(OpenSSL REQUIRED) 
#if( OpenSSL_FOUND )
#    include_directories(${OPENSSL_INCLUDE_DIRS})
#    link_directories(${OPENSSL_LIBRARIES})
#    message(STATUS "Using OpenSSL ${OPENSSL_VERSION} with directories \"${OPENSSL_INCLUDE_DIRS}\"")
#endif()

# Packaging of plugin
if(EXISTS ${OUTPUT_DIR}/${excutable_filename})
	file(MD5 ${OUTPUT_DIR}/${excutable_filename} exe_md5)
	file(SHA1 ${OUTPUT_DIR}/${excutable_filename} exe_sha1)
	file(SHA512 ${OUTPUT_DIR}/${excutable_filename} exe_sha512)
	set(excutable_details "<excutable os=\"${CMAKE_SYSTEM_NAME}\">\n\t\t<filename>${excutable_filename}</filename>\n\t\t<checksum type=\"md5\">${exe_md5}</checksum>\n\t\t<checksum type=\"sha1\">${exe_sha1}</checksum>\n\t\t<checksum type=\"sha512\">${exe_sha512}</checksum>\n\t</excutable>")
else()
	message(WARNING "The excutable does not exist! Skipping zip.")
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/resources/plugin.xml" xml)
STRING(REPLACE "<!-- Excutable -->" "${excutable_details}" xml_out ${xml})
file(WRITE "${xml_file}" "${xml_out}")

#This is just to make sure that the zip gets generated.
add_custom_target(xml_creation
    ALL COMMAND cmake -E echo "XML Created"
)


message("Excutable: ${excutable_filename}")
message("Directory: ${OUTPUT_DIR}")
add_custom_target(create_zip
	ALL COMMAND ${CMAKE_COMMAND} -E tar "cfv" "PolyMost.zip" --format=zip "${excutable_filename}" "${xml_file}"
	WORKING_DIRECTORY ${OUTPUT_DIR})
add_dependencies(create_zip PolyMost xml_creation)
