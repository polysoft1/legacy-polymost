set(CHKSUM_PREFIX1 "\t\t<checksum type=\"")
set(CHKSUM_PREFIX2 "\">")
set(CHKSUM_SUFFIX "</checksum>\n")

if(NOT DEFINED EXECUTABLE)
    set(EXECUTABLE CACHE STRING "EXECUTABLE")
endif()

if(NOT DEFINED OUT_DIR)
    set(OUT_DIR CACHE STRING "OUT_DIR")
endif()
if(NOT DEFINED XML_FILE)
    set(XML_FILE CACHE STRING "XML_FILE")
endif()
if(NOT DEFINED HOME)
	set(HOME CACHE STRING "HOME")
endif()

set(EXEC_PATH ${OUT_DIR}/${EXECUTABLE})

message("Path: ${EXEC_PATH}")

if(EXISTS ${EXEC_PATH})
	file(MD5 ${EXEC_PATH} exe_md5)
	set(md5_chksum "${CHKSUM_PREFIX1}md5${CHKSUM_PREFIX2}${exe_md5}${CHKSUM_SUFFIX}")

	file(SHA1 ${EXEC_PATH} exe_sha1)
	set(sha1_chksum "${CHKSUM_PREFIX1}sha1${CHKSUM_PREFIX2}${exe_sha1}${CHKSUM_SUFFIX}")

	file(SHA512 ${EXEC_PATH} exe_sha512)
	set(sha512_chksum "${CHKSUM_PREFIX1}sha512${CHKSUM_PREFIX2}${exe_sha512}${CHKSUM_SUFFIX}")

	set(excutable_details "<excutable os=\"${CMAKE_SYSTEM_NAME}\">\n\t\t<filename>${EXECUTABLE}</filename>\n${md5_chksum}${sha1_chksum}${sha512_chksum}\n\t</excutable>")
else()
		message(WARNING "The excutable does not exist! Skipping hashes.")
endif()

file(READ "${HOME}/resources/plugin.xml" xml)
STRING(REPLACE "<!-- Excutable -->" "${excutable_details}" xml_out ${xml})
file(WRITE "${OUT_DIR}/${XML_FILE}" "${xml_out}")
