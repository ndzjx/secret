
#ifndef MOD_META
#define MOD_META

#include "mod_email.h"
#include "mod_encrypt.h"
#include "mod_parallel.h"
#include <iguana/json.hpp>

namespace secret {

//////////////////////////////////////////////////////////////////////////
// File META
struct file_meta
{
	string		id		= ""	;
	string		tag		= ""	;
	int64_t		bytes	= 0		;
	int64_t		beg		= 0		;
	int64_t		end		= 0		;
};

REFLECTION( file_meta, id, tag, bytes, beg, end ) ;

template<class T>
inline auto meta_to_json( T&& obj )
{
	iguana::string_stream ss ;

	try
	{
		iguana::json::to_json( ss, obj ) ;
	}

	catch ( ... )
	{

	}

	return ss.str() ;
}

inline int meta_from_json( file_meta& obj, const char* str )
{
	try
	{
		if ( strlen( str ) == 0 )
		{
			return 1 ;
		}
		
		iguana::json::from_json( obj, str ) ;
	}

	catch ( ... )
	{
		return 1 ;
	}

	return 0 ;
}

inline int meta_from_file( file_meta& obj, const char* file )
{
	using namespace boost::filesystem ;
	using namespace secret ;

	obj.id = hash_md5( file ) ;
	if ( obj.id.empty() )
	{
		return 1 ;
	}

	obj.tag = system_complete( file ).filename().generic_string() ;
	if ( obj.tag.empty() )
	{
		return 2 ;
	}

	obj.bytes = file_size( file ) ;
	if ( obj.bytes == 0 )
	{
		return 3 ;
	}

	obj.beg = 0 ;
	obj.end = obj.bytes ;
	return 0 ;
}

inline auto meta_split_chunk( const file_meta& fm, int64_t chunk_size )
{
	vector<file_meta> ret ;
	
	auto remain_bytes = fm.bytes ;
	for ( int64_t b = 0 ; remain_bytes ; b += chunk_size )
	{
		if ( remain_bytes <= chunk_size )
		{
			auto chunk = fm ;
			chunk.beg = b ;
			chunk.end = b + remain_bytes ;
			ret.emplace_back( chunk ) ;
			break ;
		}

		auto chunk = fm ;
		chunk.beg = b ;
		chunk.end = b + chunk_size ;
		ret.emplace_back( chunk ) ;
		
		remain_bytes -= chunk_size ;
	}

	return ret ;
}

//////////////////////////////////////////////////////////////////////////
// Service META
struct service_meta
{
	string		user	= ""	;
	string		pawd	= ""	;
	string		smtp	= ""	;
	string		pop3	= ""	;
	int64_t		mails	= 0		;
	int64_t		octets	= 0		;
};

REFLECTION( service_meta, user, pawd, smtp, pop3, mails, octets ) ;

//////////////////////////////////////////////////////////////////////////

inline auto service_update( service_meta& service )
{
	auto stat = email_stat( service.pop3.c_str(), service.user.c_str(), service.pawd.c_str() ) ;
	if ( stat.first == 0 && stat.second == 0 )
	{
		return decltype( service.mails )( 0 ) ;
	}

	if ( stat.first != service.mails || stat.second != service.octets )
	{
		auto old = service.mails ;
		service.mails = stat.first ;
		service.octets = stat.second ;
		return stat.first - old ;
	}

	return decltype( service.mails )( 0 ) ;
}

//////////////////////////////////////////////////////////////////////////

inline int file_to_service( const service_meta& service, file_meta fm, const char* file, const char* to )
{
	auto subject = meta_to_json( fm ) ;
	if ( subject.empty() )
	{
		return 1 ;
	}
	
	boost::algorithm::replace_all( subject, "\"", "\"\"" ) ;
	return email_send( service.smtp.c_str(), service.user.c_str(), service.pawd.c_str(), to, subject.c_str(), file, fm.beg, fm.end - fm.beg ) ;
}

//////////////////////////////////////////////////////////////////////////

}

#endif
