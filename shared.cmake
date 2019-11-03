project(PolyMost)

include_directories(${POLYCHAT})
set(executable_filename ${CMAKE_SHARED_LIBRARY_PREFIX}PolyMost${CMAKE_SHARED_LIBRARY_SUFFIX})
set(xml_file ${OUTPUT_DIR}/plugin.xml)

ADD_LIBRARY(PolyMost SHARED ${src})

get_target_property(DYN_INCLUDE_DIRS PolyMost INCLUDE_DIRECTORIES)
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

add_custom_target(create_zip
	ALL COMMAND ${CMAKE_COMMAND} -E tar "cfv" "PolyMost.zip" --format=zip "${executable_filename}" "${xml_file}"
	WORKING_DIRECTORY ${OUTPUT_DIR}
)
add_dependencies(create_zip xml_creation PolyMost)
