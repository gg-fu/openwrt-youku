cmake_minimum_required(VERSION 2.8)


aux_source_directory(./src/ DIRSRCS)

ADD_EXECUTABLE(MQTT-Client ${DIRSRCS})
target_include_directories(MQTT-Client PUBLIC
    "include")
TARGET_LINK_LIBRARIES(MQTT-Client dl ubus ubox blobmsg_json daemon sqlite3 json-c ssl crypto pthread -L"./lib" event)
install(TARGETS MQTT-Client RUNTIME DESTINATION bin)
