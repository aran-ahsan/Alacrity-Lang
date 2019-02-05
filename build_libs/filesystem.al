# lib file for including filesystem support in projects

# prepending $ to any string will fetch treat the string as variable and fetch its value
# similarly, prepending % will execute it as command and fetch the standard output of it
# and prepending # will execute it as command and fetch the return value of it
# no quotes required ( depending on string )

if( "${CC}" == "" ) {
	fail( "CC environment variable is not set!\n" )
}

if( "${LANGUAGE}" != "c++" ) {
	fail( "Only C++ supports filesystems\n" )
}

if( "${STD_VERSION}" < 17 ) {
	fail( "Minimum required standard version for filesystem library is c++17\n" )
}

load_file( "tests/cpp_filesystem_support" )

if( "${CPP_FILESYSTEM_SUPPORT}" == no || "${CPP_FILESYSTEM_SUPPORT}" == "" ) {
	fail( "The filesystem library is not supported by your compiler!\n" )
}

if( "${OS}" == OS_OSX ) {
	builds.add_cxx_inc_dirs( "-I/usr/local/opt/llvm/include" )
	builds.add_cxx_lib_dirs( "-L/usr/local/opt/llvm/lib" )
	builds.add_cxx_lib_flags( "-lc++fs" )
} elif( "${OS}" == OS_LINUX ) {
	builds.add_cxx_lib_flags( "-lstdc++fs" )
}