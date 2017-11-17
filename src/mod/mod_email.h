
#ifndef MOD_EMAIL
#define MOD_EMAIL

#include "mod_std.h"
#include "mod_boost.h"
#include "mod_file.h"

namespace secret {

#ifdef _WIN32
#include <windows.h>

inline int exec_proxy( const char* run, HANDLE hWritePipe = NULL, HANDLE hReadPipe = NULL )
{
	shared_ptr<char> cmd( new char[ strlen( run ) + 1 ], []( char* p ){ delete []p ; } ) ;
	if ( cmd == nullptr )
	{
		return 1 ;
	}
	
	if ( strcpy_s( cmd.get(), strlen( run ) + 1, run ) != 0 )
	{
		return 2 ;
	}
	
	PROCESS_INFORMATION pi = { 0 } ;
	STARTUPINFO si = { 0 } ;
	si.cb = sizeof( si ) ;

	if ( hReadPipe || hWritePipe )
	{
		si.dwFlags = STARTF_USESTDHANDLES ;
		si.hStdOutput = hWritePipe ;
		si.hStdInput = hReadPipe ;
	}

	if ( CreateProcessA( NULL, cmd.get(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ) != TRUE )
	{
		return 3 ;
	}

	DWORD dwCode = 4 ;
	WaitForSingleObject( pi.hProcess, INFINITE ) ;
	GetExitCodeProcess( pi.hProcess, &dwCode ) ;
	CloseHandle( pi.hProcess ) ;
	CloseHandle( pi.hThread ) ;
	return dwCode ;
}

inline auto email_send(
	const char* smtp,
	const char* user,
	const char* password,
	const char* to,
	const char* subject,
	const char* file )
{
	using namespace boost::filesystem ;
	
	string raw = R"(python)" ;
	raw += R"( )" ;
	raw += ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "email_send.py" ) ).generic_string() ;
	raw += R"( )" ;
	raw += smtp ;
	raw += R"( )" ;
	raw += password ;
	raw += R"( )" ;
	raw += user ;
	raw += R"( )" ;
	raw += to ;
	raw += R"( )" ;
	raw += subject ;
	raw += R"( )" ;
	raw += file ;
	return exec_proxy( raw.c_str() ) ;
}

inline auto email_stat(
	const char* pop3,
	const char* user,
	const char* password )
{
	using namespace boost::filesystem ;

	pair<int64_t, int64_t> ret ;
	
	string raw = R"(python)" ;
	raw += R"( )" ;
	raw += ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "email_stat.py" ) ).generic_string() ;
	raw += R"( )" ;
	raw += pop3 ;
	raw += R"( )" ;
	raw += user ;
	raw += R"( )" ;
	raw += password ;

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
	
	string raw = R"(python)" ;
	raw += R"( )" ;
	raw += ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "email_subject.py" ) ).generic_string() ;
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
