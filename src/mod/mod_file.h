
#ifndef MOD_FILE
#define MOD_FILE

#include "mod_encrypt.h"

namespace secret {
	
inline auto file_plan_remove( const char* file )
{
	return shared_ptr<void>( nullptr, [file](void*){ remove( file ) ; } ) ;
}

inline auto file_size( const char* file )
{
	long int ret = 0 ;

	try
	{
		FILE* ptr = nullptr ;
		fopen_s( &ptr, file, "r" ) ;
		if ( ptr == nullptr )
		{
			throw 10 ;
		}
		
		fseek( ptr, 0, SEEK_END ) ;
		ret = ftell( ptr ) ;
		fclose( ptr ) ;
	}

	catch ( ... )
	{

	}

	return ret ;
}

inline auto file_write_string( const char* file, const char* str )
{
	try
	{
		ofstream fs( file ) ;
		ostream oss( fs.rdbuf() ) ;
		oss << str ;
	}

	catch ( ... )
	{
		return false ;
	}

	return true ;
}

inline auto file_transform_base64( const char* file )
{
	try
	{
		ifstream fs( file, ios_base::in | ios_base::binary ) ;
		istreambuf_iterator<char> beg( fs ), end ;
		return base64_encode( vector<char>( beg, end ) ) ;
	}

	catch ( ... )
	{

	}

	return string() ;
}

}

#endif
