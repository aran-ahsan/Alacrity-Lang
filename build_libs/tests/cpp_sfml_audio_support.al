print( "{b}Checking for SFML audio support{0} ... " )
if( "${OS}" == OS_OSX ) {
	eval( "#{${CC} -std=c++11 cpp_sfml_audio_support.cpp -o /dev/null -lsfml-audio 2>/dev/null}" )
} elif( "${OS}" == OS_LINUX ) {
	eval( "#{${CC} -std=c++11 cpp_sfml_audio_support.cpp -o /dev/null -lsfml-audio 2>/dev/null}" )
} elif( "${OS}" == OS_BSD ) {
	eval( "#{${CC} -std=c++11 cpp_sfml_audio_support.cpp -o /dev/null -I/usr/local/include -L/usr/local/lib -lsfml-audio 2>/dev/null}" )
}
if( "${EXIT_STATUS}" == 0 ) {
	CPP_SFML_AUDIO_SUPPORT = yes
	print( "{g}yes{0}\n" )
} else {
	CPP_SFML_AUDIO_SUPPORT = no
	print( "{r}no{0}\n" )
}