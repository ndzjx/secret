
#ifndef MOD_META
#define MOD_META

#include <mod_email.h>
#include <mod_encrypt.h>
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

//////////////////////////////////////////////////////////////////////////
// Service META
struct service_meta
{
	string		smtp	= ""	;
	string		pop3	= ""	;
	string		user	= ""	;
	string		pawd	= ""	;
	size_t		index	= 0		;
	int64_t		mails	= 0		;
	int64_t		octets	= 0		;
};

REFLECTION( service_meta, smtp, pop3, user, pawd, index, mails, octets ) ;

using service_cloud_t = unordered_map< size_t, service_meta > ;
using service_index_t = unordered_map< string, vector< pair< pair< size_t, int64_t >, file_meta > > > ;

//////////////////////////////////////////////////////////////////////////

inline auto service_update( service_meta& service )
{
	if ( service.index == 0 )
	{
		service.index = std::hash<string>()( service.smtp + service.pop3 + service.user ) ;
	}

	auto stat = email_stat( service.pop3.c_str(), service.user.c_str(), service.pawd.c_str() ) ;
	if ( stat.first != service.mails || stat.second != service.octets )
	{
		service.mails = stat.first ;
		service.octets = stat.second ;
		return true ;
	}

	return false ;
}

inline void service_makeindex( const service_meta& service, service_index_t& index )
{
	for ( decltype( service.mails ) i = 1 ; i <= service.mails ; ++i )
	{
		auto subject = email_subject( service.pop3.c_str(), service.user.c_str(), service.pawd.c_str(), i ) ;

		file_meta fm ;
		meta_from_json( fm, subject.c_str() ) ;
		if ( fm.id.empty() )
		{
			continue ;
		}

		if ( fm.bytes == 0 || fm.end == 0 )
		{
			continue ;
		}

		index[ fm.id ].push_back( make_pair( make_pair( service.index, i ), fm ) ) ;
	}
}

inline auto file_from_single_chunk(
	service_cloud_t& cloud,
	service_index_t& index,
	const char* filehash,
	const char* file )
{
	// 索引里有这个文件
	if ( index.find( filehash ) == index.end() )
	{
		return false ;
	}

	for ( auto&& chunk : index[ filehash ] )
	{
		const auto& service = cloud[ chunk.first.first ] ;
		auto uidl = chunk.first.second ;
		auto& fm = chunk.second ;
		if ( fm.bytes != ( fm.end - fm.beg ) )
		{
			continue ;
		}
		
		if ( file_create( file, fm.bytes ) != 0 )
		{
			break ;
		}

		if ( email_recv(
			service.pop3.c_str(),
			service.user.c_str(),
			service.pawd.c_str(),
			uidl,
			file,
			0, fm.bytes, 0 ) != 200 )
		{
			continue ;
		}

		return true ;
	}

	return false ;
}

inline int file_to_service( const service_meta& service, const char* file, const char* to )
{
	file_meta fm ;
	if ( meta_from_file( fm, file ) != 0 )
	{
		return 1 ;
	}

	auto subject = meta_to_json( fm ) ;
	if ( subject.empty() )
	{
		return 2 ;
	}

	boost::algorithm::replace_all( subject, "\"", "\"\"" ) ;
	return email_send( service.smtp.c_str(), service.user.c_str(), service.pawd.c_str(), to, subject.c_str(), file ) ;
}

//////////////////////////////////////////////////////////////////////////

}

#endif
