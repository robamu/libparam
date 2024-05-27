add_library(libparam)

target_sources(libparam PRIVATE
  src/mpack/mpack.c
  src/objstore/objstore.c
  src/param/param.c
  src/param/param_client.c
  src/param/param_queue.c
  src/param/param_serializer.c
  src/param/param_server.c
  src/param/param_string.c
  src/param/param_wildcard.c
  src/param/commands/param_commands.c
  src/param/commands/param_commands_client.c
  src/param/list/param_list.c
  src/param/list/param_list_store_vmem.c
)

# Slash required additional dependency.
# src/param/list/param_list_slash.c
# src/param/param_slash.c
# src/objstore/objstore_slash.c
# src/param/commands/param_commands_slash.c
# src/param/list/param_list_store_slash.c

target_include_directories(libparam PUBLIC
  include
)
target_include_directories(libparam PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(libparam.h.in libparam.h)

target_link_libraries(libparam PRIVATE csp)
