project(PolyMost)

include_directories(${POLYCHAT})
set(executable_filename ${CMAKE_SHARED_LIBRARY_PREFIX}PolyMost${CMAKE_SHARED_LIBRARY_SUFFIX})
set(xml_file ${OUTPUT_DIR}/plugin.xml)

set(FULL_SRC ${src} ${PROTO_SRC})
ADD_LIBRARY(PolyMost SHARED ${FULL_SRC})

target_link_libraries(PolyMost ${PROTOBUF_LIBRARIES})
get_target_property(DYN_INCLUDE_DIRS PolyMost INCLUDE_DIRECTORIES)
list(APPEND DYN_INCLUDE_DIRS ${Protobuf_INCLUDE_DIRS})
list(APPEND DYN_INCLUDE_DIRS ${PROJECT_SOURCE_DIR})
list(APPEND DYN_INCLUDE_DIRS ${PROJECT_BINARY_DIR})
#if(DEFINED POLYCHAT)
	#target_include_directories(Polymost PUBLIC ${POLYCHAT_INCLUDE})
	#list(APPEND DYN_INCLUDE_DIRS ${POLYCHAT})
	#message("Including ${POLYCHAT}")
	#target_link_libraries(PolyMost ${POLYCHAT}/target/${CMAKE_SHARED_LIBRARY_PREFIX}PolyChat${CMAKE_SHARED_LIBRARY_SUFFIX})
#endif()
set_target_properties(PolyMost PROPERTIES INCLUDE_DIRECTORIES "${DYN_INCLUDE_DIRS}")

add_custom_target(xml_creation)
add_dependencies(xml_creation PolyMost)
add_custom_command(TARGET xml_creation
	POST_BUILD
	COMMAND cmake -DHOME="${CMAKE_HOME_DIRECTORY}" -DEXECUTABLE="${executable_filename}" -DOUT_DIR="${OUTPUT_DIR}" -DXML_FILE="plugin.xml" -DCMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}" -P ${CMAKE_HOME_DIRECTORY}/xmlcreate.cmake
) 

# link the app against POCO
target_link_libraries(PolyMost Poco::Foundation Poco::Util Poco::Net Poco::Data Poco::DataSQLite Poco::XML Poco::JSON Poco::NetSSL Poco::Crypto)

find_package(OpenSSL REQUIRED)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

include_directories("${OPENSSL_INCLUDE_DIR}")
#target_link_libraries(PolyMost "${OPENSSL_LIBRARIES}")

include_directories(${CURLPP_INCLUDE})
include_directories(${CURL_INCLUDE_DIR})

#target_link_libraries(PolyMost ${CURLPP_LIB}/curlpp.lib)
#target_link_libraries(PolyMost ${CURL_LIBRARY}/libcurl-d_imp.lib)

if(false AND WIN32)
	foreach(DLL POCO_MODULES)
		SET(DLL Poco${DLL})
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			SET(DLL ${DLL}d)
		endif()
		add_custom_command(TARGET PolyMost POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_PREFIX_PATH}/bin/${DLL}.dll"
			$<TARGET_FILE_DIR:PolyMost>)
	endforeach(DLL)
endif()

add_custom_target(create_zip
	ALL COMMAND ${CMAKE_COMMAND} -E tar "cfv" "PolyMost.zip" --format=zip "${executable_filename}" "${xml_file}"
	WORKING_DIRECTORY ${OUTPUT_DIR}
)
add_dependencies(create_zip xml_creation PolyMost)
