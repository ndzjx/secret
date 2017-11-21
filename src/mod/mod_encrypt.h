
#ifndef MOD_ENCRYPT
#define MOD_ENCRYPT

#include "mod_std.h"
#include "mod_boost.h"
#include "mod_pipe.h"
#include "mod_file.h"

namespace secret {

inline string hash_md5( const char* file )
{
	using namespace boost::filesystem ;

	string ret ;
	
	string raw = ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "hash_md5.exe" ) ).generic_string() ;
	
	raw += R"( ")" ;
	raw += file ;
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

template<class T>
inline auto base64_encode( T&& param )
{
	constexpr char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;
	auto Data = param.data() ;
	const size_t DataByte = param.size() ;

	string ret ;
	unsigned char Tmp[ 4 ] = { 0 } ;
	for ( size_t i = 0 ; i < size_t( DataByte / 3 ) ; ++i )
    {
		Tmp[ 1 ] = *Data++ ;
		Tmp[ 2 ] = *Data++ ;
		Tmp[ 3 ] = *Data++ ;
        ret += EncodeTable[ Tmp[ 1 ] >> 2 ] ;
        ret += EncodeTable[ ( ( Tmp[ 1 ] << 4 ) | ( Tmp[ 2 ] >> 4 ) ) & 0x3F ] ;
        ret += EncodeTable[ ( ( Tmp[ 2 ] << 2 ) | ( Tmp[ 3 ] >> 6 ) ) & 0x3F ] ;
        ret += EncodeTable[ Tmp[ 3 ] & 0x3F ] ;
    }
	
	auto Mod = size_t( DataByte % 3 ) ;
    if ( Mod == 1 )
    {
		Tmp[ 1 ] = *Data++ ;
		ret += EncodeTable[ ( Tmp[ 1 ] & 0xFC ) >> 2 ] ;
		ret += EncodeTable[ ( ( Tmp[ 1 ] & 0x03 ) << 4 ) ] ;
		ret += "==" ;
		return ret ;
    }

    if ( Mod == 2 )
    {
		Tmp[ 1 ] = *Data++ ;
		Tmp[ 2 ] = *Data++ ;
        ret += EncodeTable[ ( Tmp[ 1 ] & 0xFC ) >> 2 ] ;
        ret += EncodeTable[ ( ( Tmp[ 1 ] & 0x03 ) << 4 ) | ( ( Tmp[ 2 ] & 0xF0 ) >> 4 ) ] ;
        ret += EncodeTable[ (( Tmp[ 2 ] & 0x0F ) << 2 ) ] ;
        ret += '=' ;
		return ret ;
    }
	
	return decltype( ret )() ;
}

template<class T = vector<char>>
inline auto base64_decode( const char* str )
{
    constexpr char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    } ;
	
	T ret ;
	auto Data = str ;
	const auto DataByte = strlen( str ) ;
	size_t nValue = 0 ;
	size_t i = 0 ;
	while ( i < DataByte )
    {
		nValue = DecodeTable[ *Data++ ] << 18 ;
		nValue += DecodeTable[ *Data++ ] << 12 ;
		ret.push_back( char( ( nValue & 0x00FF0000 ) >> 16 ) ) ;
		
		if ( *Data != '=' )
		{
			nValue += DecodeTable[ *Data++ ] << 6 ;
			ret.push_back( char( ( nValue & 0x0000FF00 ) >> 8 ) ) ;
			
			if ( *Data != '=' )
			{
				nValue += DecodeTable[ *Data++ ] ;
				ret.push_back( char( nValue & 0x000000FF ) ) ;
			}
		}
		
		i += 4 ;
	}
	
	return ret ;
}

}

#endif
