SET(CMAKE_LIBRARY_PATH_FLAG "-LIBPATH:")
SET(CMAKE_LINK_LIBRARY_FLAG "")
SET(WIN32 1)
IF(CMAKE_VERBOSE_MAKEFILE)
  SET(CMAKE_CL_NOLOGO)
ELSE(CMAKE_VERBOSE_MAKEFILE)
  SET(CMAKE_CL_NOLOGO "/nologo")
ENDIF(CMAKE_VERBOSE_MAKEFILE)
# create a shared C++ library
SET(CMAKE_CXX_CREATE_SHARED_LIBRARY 
 "link ${CMAKE_CL_NOLOGO} ${CMAKE_START_TEMP_FILE}  /out:<TARGET> /dll  <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> ${CMAKE_END_TEMP_FILE}")

SET(CMAKE_CXX_CREATE_SHARED_MODULE ${CMAKE_CXX_CREATE_SHARED_LIBRARY})

# create a C shared library
SET(CMAKE_C_CREATE_SHARED_LIBRARY ${CMAKE_CXX_CREATE_SHARED_LIBRARY})

# create a C shared module just copy the shared library rule
SET(CMAKE_C_CREATE_SHARED_MODULE ${CMAKE_C_CREATE_SHARED_LIBRARY})


# create a C++ static library
SET(CMAKE_CXX_CREATE_STATIC_LIBRARY  "lib ${CMAKE_CL_NOLOGO} <LINK_FLAGS> /out:<TARGET> <OBJECTS> ")

# create a C static library
SET(CMAKE_C_CREATE_STATIC_LIBRARY ${CMAKE_CXX_CREATE_STATIC_LIBRARY})

# compile a C++ file into an object file
SET(CMAKE_CXX_COMPILE_OBJECT
    "<CMAKE_CXX_COMPILER>  ${CMAKE_START_TEMP_FILE} ${CMAKE_CL_NOLOGO} /TP -DWIN32 /Fo<OBJECT> <FLAGS> -c <SOURCE>${CMAKE_END_TEMP_FILE}")

# compile a C file into an object file
SET(CMAKE_C_COMPILE_OBJECT
    "<CMAKE_C_COMPILER> ${CMAKE_START_TEMP_FILE} ${CMAKE_CL_NOLOGO} -DWIN32 /Fo<OBJECT>  <FLAGS> -c <SOURCE>${CMAKE_END_TEMP_FILE}")


SET(CMAKE_C_LINK_EXECUTABLE
    "<CMAKE_C_COMPILER> ${CMAKE_CL_NOLOGO} ${CMAKE_START_TEMP_FILE} <FLAGS> <OBJECTS> /Fe<TARGET> -link <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES>${CMAKE_END_TEMP_FILE}")

SET(CMAKE_COMPILE_RESOURCE "rc <FLAGS> /fo<OBJECT> <SOURCE>")

SET(CMAKE_CXX_LINK_EXECUTABLE
    "<CMAKE_CXX_COMPILER> ${CMAKE_CL_NOLOGO} ${CMAKE_START_TEMP_FILE} <FLAGS> <OBJECTS> /Fe<TARGET> -link <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES>${CMAKE_END_TEMP_FILE}")

SET(CMAKE_CREATE_WIN32_EXE /subsystem:windows)
SET(CMAKE_CREATE_CONSOLE_EXE /subsystem:console)

# default to Debug builds
#SET(CMAKE_BUILD_TYPE_INIT Debug)
SET(CMAKE_BUILD_TYPE_INIT Release)
SET (CMAKE_CXX_FLAGS_INIT "/W3 /Zm1000 /GX /GR")
SET (CMAKE_CXX_FLAGS_DEBUG_INIT "/MDd /Zi /Od /GZ")
SET (CMAKE_CXX_FLAGS_MINSIZEREL_INIT "/MD /O1")
SET (CMAKE_CXX_FLAGS_RELEASE_INIT "/MD /O2")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MD /Zi /O2")
SET (CMAKE_C_FLAGS_INIT "/W3 /Zm1000")
SET (CMAKE_C_FLAGS_DEBUG_INIT "/MDd /Zi /Od /GZ")
SET (CMAKE_C_FLAGS_MINSIZEREL_INIT "/MD /O1")
SET (CMAKE_C_FLAGS_RELEASE_INIT "/MD /O2")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MD /Zi /O2")


SET(CMAKE_C_STANDARD_LIBRARIES_INIT "kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib")
SET(CMAKE_CXX_STANDARD_LIBRARIES_INIT "${CMAKE_C_STANDARD_LIBRARIES_INIT}")

# executable linker flags
SET (CMAKE_LINK_DEF_FILE_FLAG "/DEF:")
SET (CMAKE_EXE_LINKER_FLAGS_INIT "/STACK:10000000 /INCREMENTAL:YES")
SET (CMAKE_EXE_LINKER_FLAGS_DEBUG_INIT "/debug")
SET (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO_INIT "/debug")

SET (CMAKE_SHARED_LINKER_FLAGS_INIT ${CMAKE_EXE_LINKER_FLAGS_INIT})
SET (CMAKE_SHARED_LINKER_FLAGS_DEBUG_INIT ${CMAKE_EXE_LINKER_FLAGS_DEBUG_INIT})
SET (CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO_INIT ${CMAKE_EXE_LINKER_FLAGS_DEBUG_INIT})
SET (CMAKE_MODULE_LINKER_FLAGS_INIT ${CMAKE_SHARED_LINKER_FLAGS_INIT})
SET (CMAKE_MODULE_LINKER_FLAGS_DEBUG_INIT ${CMAKE_SHARED_LINKER_FLAGS_DEBUG_INIT})
SET (CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO_INIT ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO_INIT})
