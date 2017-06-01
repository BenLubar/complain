PROJECT(complain)

execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=8 --long
    WORKING_DIRECTORY "${complain_SOURCE_DIR}"
    OUTPUT_VARIABLE COMPLAIN_GIT_DESCRIPTION)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY "${complain_SOURCE_DIR}"
    OUTPUT_VARIABLE COMPLAIN_GIT_COMMIT)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=8 --exact-match
    WORKING_DIRECTORY "${complain_SOURCE_DIR}"
    RESULT_VARIABLE COMPLAIN_GIT_TAGGED_RESULT
    OUTPUT_QUIET ERROR_QUIET)

set(git_describe_tmp_h ${complain_SOURCE_DIR}/complain-git-describe.tmp.h)
set(git_describe_h ${complain_SOURCE_DIR}/complain-git-describe.h)

file(WRITE ${git_describe_tmp_h} "")

macro(git_describe_definition var)
    string(STRIP "${${var}}" GIT_DEF_VALUE)
    file(APPEND ${git_describe_tmp_h} "#define ${var} \"${GIT_DEF_VALUE}\"\n")
endmacro()

git_describe_definition(COMPLAIN_GIT_DESCRIPTION)
git_describe_definition(COMPLAIN_GIT_COMMIT)
if(${COMPLAIN_GIT_TAGGED_RESULT} EQUAL 0)
    file(APPEND ${git_describe_tmp_h} "#define COMPLAIN_GIT_TAGGED\n")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${git_describe_tmp_h} ${git_describe_h})

SET(PROJECT_SRCS
    complain.cpp
    complaints/dump_outdoor_refuse.cpp
)

SET(PROJECT_HDRS
    complain.h
    complain-git-describe.h
)

SET_SOURCE_FILES_PROPERTIES( ${PROJECT_HDRS} PROPERTIES HEADER_FILE_ONLY TRUE)

LIST(APPEND PROJECT_SRCS ${PROJECT_HDRS})

DFHACK_PLUGIN(complain ${PROJECT_SRCS} COMPILE_FLAGS_GCC "-Wall -Wextra -Werror" COMPILE_FLAGS_MSVC "/W3")
