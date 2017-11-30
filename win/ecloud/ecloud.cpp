
#include "ecloud.h"

//////////////////////////////////////////////////////////////////////////

ParallelCore& global_pc( ParallelCore* pc )
{
	static ParallelCore* g_pc = nullptr ;
	if ( g_pc == nullptr && pc != nullptr )
	{
		g_pc = pc ;
	}

	return *g_pc ;
}

ParallelCore& global_pc()
{
	return global_pc( nullptr ) ;
}

ORMapper& global_db( ORMapper* db )
{
	static ORMapper* g_db = nullptr ;
	if ( g_db == nullptr && db != nullptr )
	{
		g_db = db ;
	}

	return *g_db ;
}

ORMapper& global_db()
{
	return global_db( nullptr ) ;
}

//////////////////////////////////////////////////////////////////////////

vector<dbmeta_cloudnode> global_cloudnodes()
{
	try
	{
		dbmeta_cloudnode model ;
		return global_db().Query( model ).ToVector() ;
	}

	catch ( ... )
	{

	}

	return vector<dbmeta_cloudnode>() ;
}

//////////////////////////////////////////////////////////////////////////

void global_cloudnodes_update( std::shared_ptr<void> fina )
{
	auto task_finally = [ fina ]( auto ptr_service, auto ptr_news )
	{
		try
		{
			auto&& service = *ptr_service ;
			auto&& news = *ptr_news ;

			if ( news )
			{
				dbmeta_cloudnode node ;
				service.mails += news ;
				node.from_meta( service ) ;
				global_db().Update( node ) ;
			}
		}

		catch ( ... )
		{

		}
	} ;

	auto task_insert_index = [ task_finally ]( auto self, auto ptr_service, auto ptr_news )
	{
		try
		{
			auto&& service = *ptr_service ;
			auto&& news = *ptr_news ;

			const auto i = service.mails + news + 1 ;
			auto subject = email_subject(
				service.pop3.c_str(),
				service.user.c_str(),
				service.pawd.c_str(), i ) ;
			
			if ( subject.empty() )
			{
				global_pc().post( std::bind( task_finally, ptr_service, ptr_news ) ) ;
				return ;
			}

			++news ;
			global_pc().post( std::bind( self, self, ptr_service, ptr_news ) ) ;

			file_meta fm ;
			if ( meta_from_json( fm, subject.c_str() ) != 0 )
			{
				return ;
			}

			if ( fm.id.empty() || fm.bytes == 0 || fm.end == 0 )
			{
				return ;
			}

			dbmeta_cloudfile index ;
			index.from_meta( fm, service.user, i ) ;
			global_db().Insert( index ) ;
		}

		catch ( ... )
		{

		}
	} ;

	auto task_check_update = [ task_insert_index ]
	{
		try
		{
			for ( auto&& node : global_db().Query( dbmeta_cloudnode{} ).ToList() )
			{
				auto ptr_service = std::make_shared< decltype( node.to_meta() ) >( node.to_meta() ) ;
				auto ptr_news = std::make_shared< atomic_int64_t >( 0 ) ;
				global_pc().post( std::bind( task_insert_index, task_insert_index, ptr_service, ptr_news ) ) ;
			}
		}

		catch( ... )
		{

		}
	} ;

	global_pc().post( task_check_update ) ;
}

//////////////////////////////////////////////////////////////////////////

bool global_cloudnode_add( const dbmeta_cloudnode& dbnode )
{
	try
	{
		global_db().Insert( dbnode ) ;
	}

	catch ( ... )
	{

	}

	return false ;
}

//////////////////////////////////////////////////////////////////////////

bool global_cloudnode_del( const dbmeta_cloudnode& dbnode )
{
	try
	{
		global_db().Delete( dbnode ) ;
	}

	catch ( ... )
	{

	}

	return false ;
}

//////////////////////////////////////////////////////////////////////////

bool global_cloudnode_fix( const dbmeta_cloudnode& dbnode )
{
	try
	{
		global_db().Update( dbnode ) ;
	}

	catch ( ... )
	{

	}

	return false ;
}

//////////////////////////////////////////////////////////////////////////

bool global_cloudnode_get( const string& user, dbmeta_cloudnode& dbnode )
{
	try
	{
		dbmeta_cloudnode model ;
		auto field = FieldExtractor{ model } ;

		auto res = global_db().Query( model ).Where( field( model.user ) == user ).ToVector() ;
		if ( res.size() == 1 )
		{
			dbnode = res[ 0 ] ;
			return true ;
		}
	}

	catch ( ... )
	{

	}

	return false ;
}

//////////////////////////////////////////////////////////////////////////

vector<dbmeta_cloudfile> global_cloudfiles()
{
	vector<dbmeta_cloudfile> ret ;

	try
	{
		dbmeta_cloudfile model ;
		auto field = FieldExtractor{ model } ;
		
		for ( auto&& cf : global_db().Query( model ).ToVector() )
		{
			if ( global_cloudfile_exist( cf.id ) )
			{
				ret.emplace_back( cf ) ;
			}
		}
	}

	catch ( ... )
	{

	}

	return ret ;
}

//////////////////////////////////////////////////////////////////////////

bool global_cloudfile_exist( const string& id )
{
	try
	{
		dbmeta_cloudfile model ;
		auto field = FieldExtractor{ model } ;
		
		// check single chunk.
		if ( global_db().Query( model ).Where( 
			field( model.id ) == id &&
			field( model.beg ) == string( "0" ) &&
			field( model.bytes ) == field( model.end ) ).ToVector().empty() == false )
		{
			return true ;
		}
	}

	catch ( ... )
	{

	}

	return false ;
}

//////////////////////////////////////////////////////////////////////////

bool global_cloudfile_download( const string& id, const string& file )
{
	try
	{
		dbmeta_cloudfile model_file ;
		dbmeta_cloudnode model_node ;
		auto field = FieldExtractor{ model_file, model_node } ;

		auto single_chunk = global_db().Query( model_file )
			.Join( model_node, field( model_file.service ) == field( model_node.user ) )
			.Where( 
				field( model_file.id ) == id &&
				field( model_file.beg ) == string( "0" ) &&
				field( model_file.bytes ) == field( model_file.end ) ).ToList() ;

		for ( auto&& row : single_chunk )
		{
			auto bytes = stoull( std::get< 2 >( row ).Value() ) ;
			auto number = stoull( std::get< 6 >( row ).Value() ) ;

			auto user = std::get< 7 >( row ).Value() ;
			auto pawd = std::get< 8 >( row ).Value() ;
			auto pop3 = std::get< 10 >( row ).Value() ;

			if ( file_create( file.c_str(), bytes ) != 0 )
			{
				continue ;
			}

			if ( email_recv(
				pop3.c_str(),
				user.c_str(),
				pawd.c_str(),
				number,
				file.c_str(),
				0, bytes, 0 ) != 200 )
			{
				continue ;
			}

			return true ;
		}
	}

	catch ( ... )
	{

	}

	return false ;
}

//////////////////////////////////////////////////////////////////////////
