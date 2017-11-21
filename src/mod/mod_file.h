
#ifndef MOD_FILE
#define MOD_FILE

namespace secret {
	
inline auto file_plan_remove( const char* file )
{
	return shared_ptr<void>( nullptr, [file](void*){ remove( file ) ; } ) ;
}

inline auto file_close( FILE* ptr )
{
	return shared_ptr<FILE>( ptr, fclose ) ;
}

inline auto file_open( const char* file, const char* mode = "r" )
{
	FILE* ptr = nullptr ;
	fopen_s( &ptr, file, mode ) ;
	return file_close( ptr ) ;
}

inline auto file_handle( void* file, const char* mode = "r" )
{
	return file_close( _fdopen( _open_osfhandle( (intptr_t)file, 0x8000 ), mode ) ) ;
}

inline auto file_size( FILE* ptr )
{
	auto now = _ftelli64( ptr ) ;
	_fseeki64( ptr, 0, SEEK_END ) ;
	auto ret = _ftelli64( ptr ) ;
	_fseeki64( ptr, now, SEEK_SET ) ;
	return ret ;
}

inline auto file_size( const char* file )
{
	auto ptr = file_open( file ) ;
	if ( ptr )
	{
		return file_size( ptr.get() ) ;
	}
	
	return decltype( file_size( (FILE*)nullptr ) )( 0 ) ;
}

inline auto file_create( const char* file, size_t bytes )
{
	auto ptr = file_open( file, "w" ) ;
	if ( ptr )
	{
		_fseeki64( ptr.get(), bytes, SEEK_SET ) ;
		fputc( 0, ptr.get() ) ;
		return 0 ;
	}

	return 1 ;
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

template<class T>
inline auto file_transform( const char* file, T algo )
{
	try
	{
		ifstream fs( file, ios_base::in | ios_base::binary ) ;
		istreambuf_iterator<char> beg( fs ), end ;
		return algo( vector<char>( beg, end ) ) ;
	}

	catch ( ... )
	{

	}

	return string() ;
}

}

#endif
