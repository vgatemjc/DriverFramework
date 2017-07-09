############################################################################
# Cross-compilation Toolchain File (CMAKE_TOOLCHAIN_FILE)
# for Intel EDISON
# Author: Jae Chul Moon (vgatemjc@gmail.com)
#
############################################################################

OPTION(USE_MRAA_LIB "use mraa library" ON)

add_definitions(
	-D__DF_EDISON
	-D__DF_LINUX
)

if(USE_MRAA_LIB)
	add_definitions(
		-D__DF_MRAA_LIB
	)
endif()

######### test DriverFramework for edison ###
# used for debug
#add_definitions(-DDF_DEBUG)

# this one is important
set(CMAKE_SYSTEM_NAME Linux)
