cmake_minimum_required(VERSION 2.8)


aux_source_directory(./src/server DIRSRCS)
aux_source_directory(./src/client DIRSRCS)
aux_source_directory(./src/common DIRSRCS)
aux_source_directory(./src/ DIRSRCS)

ADD_EXECUTABLE(COAP ${DIRSRCS})
target_include_directories(COAP PUBLIC
    "include/server"
    "include/client"
    "include/common")
TARGET_LINK_LIBRARIES(COAP dl ubus ubox blobmsg_json daemon sqlite3 json-c pthread)
install(TARGETS COAP RUNTIME DESTINATION bin)
