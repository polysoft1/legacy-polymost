project(PolyMostStatic)

set(FULL_SRC ${src} ${PROTO_SRC})
ADD_LIBRARY(PolyMostStatic STATIC ${src})
set_target_properties(PolyMostStatic PROPERTIES LINKER_LANGUAGE CXX)
get_target_property(STATIC_INCLUDE_DIRS PolyMostStatic INCLUDE_DIRECTORIES)
list(APPEND STATIC_INCLUDE_DIRS ${Protobuf_INCLUDE_DIRS})
list(APPEND STATIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR})
list(APPEND STATIC_INCLUDE_DIRS ${PROJECT_BINARY_DIR})

if(MSVC)
	SET_TARGET_PROPERTIES(PolyMostStatic PROPERTIES
		COMPILE_DEFINITIONS "PROTOBUF_USE_DLLS")
endif()

#message("INCLUDE_DIRS: ${STATIC_INCLUDE_DIRS}")

set_target_properties(PolyMostStatic PROPERTIES INCLUDE_DIRECTORIES "${STATIC_INCLUDE_DIRS}")
target_link_libraries(PolyMostStatic ${PROTOBUF_LIBRARIES})
target_link_libraries(PolyMostStatic Poco::Foundation Poco::Util Poco::Net Poco::Data Poco::DataSQLite Poco::XML Poco::JSON Poco::NetSSL Poco::Crypto)

