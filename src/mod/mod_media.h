
#ifndef MOD_MEDIA
#define MOD_MEDIA

#include "mod_std.h"
#include "mod_boost.h"
#include "mod_file.h"
#include "mod_pipe.h"

namespace secret {

#ifdef _WIN32
#include <windows.h>

inline auto media_recorder(
	int device,
	const char* file,
	const char* stop )
{
	using namespace boost::filesystem ;
	
	string raw = ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "/media/recorder.exe" ) ).generic_string() ;
	
	raw += R"( ")" ;
	raw += to_string( device ) ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += file ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += stop ;
	raw += R"(")" ;
	
	return exec_proxy( raw.c_str() ) ;
}

inline auto media_play( const char* file )
{
	using namespace boost::filesystem ;
	
	string raw = ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "/media/ffplay.exe" ) ).generic_string() ;
	
	raw += R"( ")" ;
	raw += "-autoexit" ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += "-nodisp" ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += file ;
	raw += R"(")" ;
	
	return exec_proxy( raw.c_str() ) ;
}

#endif

}

#endif
