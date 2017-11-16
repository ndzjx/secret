
#ifndef MOD_EMAIL
#define MOD_EMAIL

#include "mod_std.h"
#include "mod_boost.h"
#include "mod_file.h"

namespace secret {

#ifdef _WIN32
#include <windows.h>
inline int email_send(
	const char* smtp,
	const char* password,
	const char* from,
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
	raw += from ;
	raw += R"( )" ;
	raw += to ;
	raw += R"( )" ;
	raw += subject ;
	raw += R"( )" ;
	raw += file ;
	
	shared_ptr<char> cmd( new char[ raw.size() + 1 ], []( char* p ){ delete []p ; } ) ;
	if ( cmd == nullptr )
	{
		return 20 ;
	}
	
	if ( strcpy_s( cmd.get(), raw.size() + 1, raw.c_str() ) != 0 )
	{
		return 30 ;
	}
	
	PROCESS_INFORMATION pi = { 0 } ;
	STARTUPINFO si = { 0 } ;
	si.cb = sizeof( si ) ;
	if ( CreateProcessA( NULL, cmd.get(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) != TRUE )
	{
		return 40 ;
	}

	DWORD dwCode = 50 ;
	WaitForSingleObject( pi.hProcess, INFINITE ) ;
	GetExitCodeProcess( pi.hProcess, &dwCode ) ;
	CloseHandle( pi.hProcess ) ;
	CloseHandle( pi.hThread ) ;
	return dwCode ;
}
#endif

}

#endif
