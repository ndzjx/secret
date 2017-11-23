
#ifndef MOD_PIPE
#define MOD_PIPE

#include "mod_std.h"

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
	STARTUPINFOA si = { 0 } ;
	si.cb = sizeof( si ) ;
	si.dwFlags = STARTF_USESHOWWINDOW ;
	si.wShowWindow = SW_HIDE ;

	if ( hReadPipe || hWritePipe )
	{
		si.dwFlags |= STARTF_USESTDHANDLES ;
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

#endif

}

#endif
