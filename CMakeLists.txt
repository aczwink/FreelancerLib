cmake_minimum_required(VERSION 3.6)
project(FreelancerLib)

set(CMAKE_CXX_STANDARD 11)

#debug or release
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)

    set(CMAKE_DEBUG_POSTFIX "_d")
else()
    message(FATAL_ERROR "Unknown build type. Must be either 'Debug' or 'Release'")
endif()

#dependancies
find_library(ACSTDLIB_LIBRARY NAMES ACStdLib_d)
find_library(ACSTDLIBSTATIC_LIBRARY NAMES ACStdLibStatic_d)

add_library(FreelancerLib SHARED include/UTF.hpp include/UTFNode.hpp src/UTF.cpp)
target_link_libraries(FreelancerLib ${ACSTDLIB_LIBRARY} ${ACSTDLIBSTATIC_LIBRARY})


#---INSTALLATION---

#install target
install(TARGETS FreelancerLib EXPORT FreelancerLibConfig
        LIBRARY DESTINATION lib
        )
install(EXPORT FreelancerLibConfig DESTINATION share/FreelancerLib/cmake)

#install public headers
function(installHeaderDir sourceDir targetDir)
    file(GLOB LOCAL_FILES_AND_DIRS "${sourceDir}/*")
    foreach(ITEM ${LOCAL_FILES_AND_DIRS})
        IF(IS_DIRECTORY "${ITEM}")
            LIST(APPEND LOCAL_DIRS "${ITEM}")
        ELSE()
            LIST(APPEND LOCAL_FILES "${ITEM}")
        ENDIF()
    endforeach()

    INSTALL(DIRECTORY ${LOCAL_DIRS} DESTINATION "include/${targetDir}")
    INSTALL(FILES ${LOCAL_FILES} DESTINATION include/${targetDir})
endfunction()

installHeaderDir("${PROJECT_SOURCE_DIR}/include" "FreelancerLib")