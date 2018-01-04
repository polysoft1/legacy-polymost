project(PolyMostStatic)

include_directories(${POLYCHAT})
set(executable_filename ${CMAKE_SHARED_LIBRARY_PREFIX}PolyMostStatic${CMAKE_SHARED_LIBRARY_SUFFIX})
set(xml_file ${OUTPUT_DIR}/plugin.xml)

set(FULL_SRC ${src} ${PROTO_SRC})
ADD_LIBRARY(PolyMostStatic STATIC ${FULL_SRC})
set_target_properties(PolyMostStatic PROPERTIES LINKER_LANGUAGE CXX)
if(MSVC)
	SET_TARGET_PROPERTIES(PolyMostStatic PROPERTIES
		COMPILE_DEFINITIONS "PROTOBUF_USE_DLLS")
endif()

target_link_libraries(PolyMostStatic ${PROTOBUF_LIBRARIES})
get_target_property(STATIC_INCLUDE_DIRS PolyMostStatic INCLUDE_DIRECTORIES)
list(APPEND STATIC_INCLUDE_DIRS ${Protobuf_INCLUDE_DIRS})
list(APPEND STATIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR})
list(APPEND STATIC_INCLUDE_DIRS ${PROJECT_BINARY_DIR})
#if(DEFINED POLYCHAT)
	#target_include_directories(PolyMostStatic PUBLIC ${POLYCHAT_INCLUDE})
	#list(APPEND STATIC_INCLUDE_DIRS ${POLYCHAT})
	#message("Including ${POLYCHAT}")
	#target_link_libraries(PolyMostStatic ${POLYCHAT}/target/${CMAKE_SHARED_LIBRARY_PREFIX}PolyChat${CMAKE_SHARED_LIBRARY_SUFFIX})
#endif()
set_target_properties(PolyMostStatic PROPERTIES INCLUDE_DIRECTORIES "${STATIC_INCLUDE_DIRS}")

# link the app against POCO
target_link_libraries(PolyMostStatic Poco::Foundation Poco::Util Poco::Net Poco::Data Poco::DataSQLite Poco::XML Poco::JSON Poco::NetSSL Poco::Crypto)

find_package(OpenSSL REQUIRED)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

include_directories("${OPENSSL_INCLUDE_DIR}")

include_directories(${CURLPP_INCLUDE})
include_directories(${CURL_INCLUDE_DIR})
message("Including ${CURL_INCLUDE_DIR}")

target_link_libraries(PolyMostStatic ${CURLPP_LIB}/libcurlpp.lib)
target_link_libraries(PolyMostStatic ${CURL_LIBRARY}/libcurl-d_imp.lib)
