
# Macro to add source files to Variables
# WARN: Dont use function to replace macro.
macro(add_source_directories)
    FOREACH(dir_name ${SOURCE_DIRS})
        file(GLOB_RECURSE FUNC_ASM_SOURCES "${CMAKE_SOURCE_DIR}/${dir_name}/*.S")
        file(GLOB_RECURSE FUNC_C_SOURCES "${CMAKE_SOURCE_DIR}/${dir_name}/*.c")
        list(APPEND ASM_SOURCES ${FUNC_ASM_SOURCES})
        list(APPEND C_SOURCES ${FUNC_C_SOURCES})
    ENDFOREACH()
endmacro()
