
#ifndef MOD_EMAIL
#define MOD_EMAIL

#include "mod_std.h"
#include "mod_boost.h"
#include "mod_file.h"
#include "mod_pipe.h"

namespace secret {

#ifdef _WIN32
#include <windows.h>

inline auto email_send(
	const char* smtp,
	const char* user,
	const char* password,
	const char* to,
	const char* subject,
	const char* file )
{
	using namespace boost::filesystem ;
	
	string raw = ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "email_send.exe" ) ).generic_string() ;
	
	raw += R"( ")" ;
	raw += smtp ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += password ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += user ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += to ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += subject ;
	raw += R"(")" ;
	
	raw += R"( ")" ;
	raw += file ;
	raw += R"(")" ;
	
	return exec_proxy( raw.c_str() ) ;
}

inline auto email_stat(
	const char* pop3,
	const char* user,
	const char* password )
{
	using namespace boost::filesystem ;

	pair<int64_t, int64_t> ret ;

	string raw = ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "email_stat.exe" ) ).generic_string() ;
	
	raw += R"( ")" ;
	raw += pop3 ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += user ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += password ;
	raw += R"(")" ;

	SECURITY_ATTRIBUTES sa = { 0 } ;
	sa.nLength = sizeof( sa ) ;
	sa.bInheritHandle = TRUE ;
	sa.lpSecurityDescriptor = NULL ;

	HANDLE hReadPipe = NULL ;
	HANDLE hWritePipe = NULL ;
	if ( CreatePipe( &hReadPipe, &hWritePipe, &sa, 0 ) != TRUE )
	{
		return ret ;
	}
	
	if ( exec_proxy( raw.c_str(), hWritePipe ) != 200 )
	{
		CloseHandle( hReadPipe ) ;
		CloseHandle( hWritePipe ) ;
		return ret ;
	}

	try
	{
		CloseHandle( hWritePipe ) ;
		auto file = file_handle( hReadPipe ) ;
		if ( file )
		{
			string str ;
			const auto len = file_size( file.get() ) ;
			str.resize( len + 1 ) ;
			fread( &str.at( 0 ), 1, len, file.get() ) ;
			str.resize( len ) ;

			int64_t a = 0 ;
			int64_t b = 0 ;
			if ( sscanf_s( str.c_str(), "%I64d,%I64d", &a, &b ) == 2 )
			{
				ret = make_pair( a, b ) ;
			}
		}
	}

	catch ( ... )
	{

	}
	
	return ret ;
}

inline auto email_subject(
	const char* pop3,
	const char* user,
	const char* password,
	int64_t uidl )
{
	using namespace boost::filesystem ;

	string ret ;

	string raw = ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "email_subject.exe" ) ).generic_string() ;
	
	raw += R"( ")" ;
	raw += pop3 ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += user ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += password ;
	raw += R"(")" ;

	raw += R"( ")" ;
	raw += to_string( uidl ) ;
	raw += R"(")" ;
	
	SECURITY_ATTRIBUTES sa = { 0 } ;
	sa.nLength = sizeof( sa ) ;
	sa.bInheritHandle = TRUE ;
	sa.lpSecurityDescriptor = NULL ;

	HANDLE hReadPipe = NULL ;
	HANDLE hWritePipe = NULL ;
	if ( CreatePipe( &hReadPipe, &hWritePipe, &sa, 0 ) != TRUE )
	{
		return ret ;
	}
	
	if ( exec_proxy( raw.c_str(), hWritePipe ) != 200 )
	{
		CloseHandle( hReadPipe ) ;
		CloseHandle( hWritePipe ) ;
		return ret ;
	}

	try
	{
		CloseHandle( hWritePipe ) ;
		auto file = file_handle( hReadPipe ) ;
		if ( file )
		{
			const auto len = file_size( file.get() ) ;
			ret.resize( len + 1 ) ;
			fread( &ret.at( 0 ), 1, len, file.get() ) ;
			ret.resize( len ) ;
		}
	}

	catch ( ... )
	{

	}

	return ret ;
}

inline auto email_recv(
	const char* pop3,
	const char* user,
	const char* password,
	int64_t uidl )
{
	using namespace boost::filesystem ;

	string ret ;
	
	string raw = R"(python)" ;
	raw += R"( )" ;
	raw += ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "email_recv.py" ) ).generic_string() ;
	raw += R"( )" ;
	raw += pop3 ;
	raw += R"( )" ;
	raw += user ;
	raw += R"( )" ;
	raw += password ;
	raw += R"( )" ;
	raw += to_string( uidl ) ;
	
	SECURITY_ATTRIBUTES sa = { 0 } ;
	sa.nLength = sizeof( sa ) ;
	sa.bInheritHandle = TRUE ;
	sa.lpSecurityDescriptor = NULL ;

	HANDLE hReadPipe = NULL ;
	HANDLE hWritePipe = NULL ;
	if ( CreatePipe( &hReadPipe, &hWritePipe, &sa, 1024 * 1024 * 10 ) != TRUE )
	{
		return ret ;
	}
	
	if ( exec_proxy( raw.c_str(), hWritePipe ) != 200 )
	{
		CloseHandle( hReadPipe ) ;
		CloseHandle( hWritePipe ) ;
		return ret ;
	}

	try
	{
		CloseHandle( hWritePipe ) ;
		auto file = file_handle( hReadPipe ) ;
		if ( file )
		{
			const auto len = file_size( file.get() ) ;
			ret.resize( len + 1 ) ;
			fread( &ret.at( 0 ), 1, len, file.get() ) ;
			ret.resize( len ) ;
		}
	}

	catch ( ... )
	{

	}

	return ret ;
}

#endif

}

#endif
