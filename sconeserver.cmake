cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFileCXX)

set(PACKAGE sconeserver)
set(PACKAGE_VERSION 0.9.11)
set(TARGET ${CMAKE_SYSTEM_PROCESSOR})
set(PACKAGE_STRING ${PACKAGE}-${PACKAGE_VERSION})

set(BIN_PATH bin)
set(LIB_PATH lib)
set(MAN_PATH share/man)
set(MOD_PATH lib/sconeserver)
set(LOG_PATH /var/log/sconeserver)
set(CONF_PATH /etc/sconeserver)
set(MODCONF_PATH ${CONF_PATH}/modules.d)
set(INC_PATH include/sconeserver)
set(DATA_PATH share/sconeserver)

function(sconeserver_module NAME)
  set_target_properties(${NAME} PROPERTIES PREFIX "")
  target_include_directories(${NAME} PRIVATE . ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
  install(TARGETS ${NAME} DESTINATION ${MOD_PATH})
  install(FILES ${NAME}.conf DESTINATION ${MODCONF_PATH}) 
endfunction()

check_include_file_cxx(unordered_map HAVE_UNORDERED_MAP)
check_include_file_cxx(tr1/unordered_map HAVE_TR1_UNORDERED_MAP)
check_include_file_cxx(ext/hash_map HAVE_EXT_HASH_MAP)
check_include_file_cxx(map HAVE_MAP)
set(HAVE_MSGHDR_MSG_CONTROL 1)

