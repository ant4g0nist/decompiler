
# Includes.
include_directories("..") # Make our includes work.
include_directories(SYSTEM
	"./include" # Make IDA SDK includes work.
)

# RetDec lldb plugin sources.
set(LLDBPLUGIN_SOURCES
	plugin.mm
)

add_library(decompiler SHARED ${LLDBPLUGIN_SOURCES})

target_link_libraries(decompiler retdec::retdec retdec::config retdec::utils retdec::deps::rapidjson)

set_target_properties(decompiler PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")

target_link_libraries(decompiler "-framework Foundation")
target_link_libraries(decompiler "-framework CoreFoundation")

target_link_libraries(decompiler
	-Wl,-force_load retdec::bin2llvmir
	-Wl,-force_load retdec::llvmir2hll
)

set_target_properties(decompiler PROPERTIES PREFIX "")
