
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

	auto task_check_number = [ task_insert_index ]( auto ptr_service, auto ptr_news )
	{
		try
		{
			auto&& service = *ptr_service ;
			
			dbmeta_cloudfile model ;
			auto field = FieldExtractor{ model } ;
			
			auto chunks = global_db()
				.Query( model )
				.Where( field( model.service ) == service.user ).ToVector() ;

			std::sort( chunks.begin(), chunks.end(), []( const auto& a, const auto& b )
			{
				return stoull( a.number ) > stoull( b.number ) ;
			} ) ;

			bool rebuild = false ;
			for ( auto&& chunk : chunks )
			{
				auto subject = email_subject(
					service.pop3.c_str(),
					service.user.c_str(),
					service.pawd.c_str(),
					stoull( chunk.number ) ) ;
					
				file_meta fm_remote ;
				if ( meta_from_json( fm_remote, subject.c_str() ) == 0 )
				{
					auto fm_local = chunk.to_meta() ;
					if (
						fm_remote.id == fm_local.id &&
						fm_remote.bytes == fm_local.bytes &&
						fm_remote.beg == fm_local.beg &&
						fm_remote.end == fm_local.end )
					{
						break ;
					}
				}
				
				rebuild = true ;
				global_db().Delete( model,
					field( model.id ) == chunk.id &&
					field( model.service ) == chunk.service &&
					field( model.number ) == chunk.number ) ;
			}

			if ( rebuild )
			{
				auto mails = global_db()
					.Query( model )
					.Where( field( model.service ) == service.user ).ToVector() ;

				auto fix = service ;
				fix.mails = mails.size() ;
					
				dbmeta_cloudnode cn ;
				cn.from_meta( fix ) ;
				global_db().Update( cn ) ;
			}

			global_pc().post( std::bind( task_insert_index, task_insert_index, ptr_service, ptr_news ) ) ;
		}

		catch( ... )
		{

		}
	} ;

	auto task_main = [ task_check_number ]
	{
		try
		{
			for ( auto&& node : global_db().Query( dbmeta_cloudnode{} ).ToVector() )
			{
				auto ptr_service = std::make_shared< decltype( node.to_meta() ) >( node.to_meta() ) ;
				auto ptr_news = std::make_shared< atomic_int64_t >( 0 ) ;
				global_pc().post( std::bind( task_check_number, ptr_service, ptr_news ) ) ;
			}
		}

		catch( ... )
		{

		}
	} ;

	global_pc().post( task_main ) ;
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
		return false ;
	}

	return true ;
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
		return false ;
	}

	return true ;
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
		return false ;
	}

	return true ;
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

bool global_cloudfile_exist_thread_chunks( const string& id, vector<dbmeta_cloudfile>& chunks )
{
	try
	{
		dbmeta_cloudfile model ;
		auto field = FieldExtractor{ model } ;
		
		auto head_chunk = global_db()
			.Query( model )
			.Where( field( model.id ) == id && field( model.beg ) == string( "0" ) ).ToVector() ;
		if ( head_chunk.empty() )
		{
			return false ;
		}

		string bytes = head_chunk[ 0 ].bytes ;
		string end = head_chunk[ 0 ].end ;
		chunks.push_back( head_chunk[ 0 ] ) ;

		while ( true )
		{
			auto chunk = global_db()
				.Query( model )
				.Where( field( model.id ) == id && field( model.beg ) == end ).ToVector() ;
			if ( chunk.empty() )
			{
				break ;
			}

			end = chunk[ 0 ].end ;
			chunks.push_back( chunk[ 0 ] ) ;
		}

		return bytes == end ;
	}

	catch ( ... )
	{

	}

	return false ;
}

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

		// check thread chunks.
		if ( global_cloudfile_exist_thread_chunks( id, vector<dbmeta_cloudfile>() ) )
		{
			return true ;
		}
	}

	catch ( ... )
	{

	}

	return false ;
}

bool global_cloudchunk_exist( const dbmeta_cloudfile& chunk )
{
	try
	{
		dbmeta_cloudfile model ;
		auto field = FieldExtractor{ model } ;
		
		auto head_chunk = global_db()
			.Query( model )
			.Where(
				field( model.id ) == chunk.id &&
				field( model.bytes ) == chunk.bytes &&
				field( model.beg ) == chunk.beg &&
				field( model.end ) == chunk.end ).ToVector() ;
		if ( head_chunk.empty() == false )
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

bool global_cloudfile_download_single_chunk( const string& id, const string& file )
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
				field( model_file.bytes ) == field( model_file.end ) ).ToVector() ;

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

bool global_cloudfile_download_multi_chunks( const string& id, const string& file )
{
	bool ret = true ;

	try
	{
		dbmeta_cloudfile model_file ;
		dbmeta_cloudnode model_node ;
		auto field = FieldExtractor{ model_file, model_node } ;

		vector<dbmeta_cloudfile> chunks ;
		if ( global_cloudfile_exist_thread_chunks( id, chunks ) == false )
		{
			throw 1 ;
		}

		if ( chunks.empty() )
		{
			throw 1 ;
		}

		if ( file_create( file.c_str(), stoull( chunks[ 0 ].bytes ) ) != 0 )
		{
			throw 1 ;
		}

		ParallelMapReduce<bool> pmr ;
		
		for ( auto&& chunk : chunks )
		{
			pmr.map( global_pc(), [ chunk, file ]( auto result )
			{
				try
				{
					dbmeta_cloudfile model_file ;
					dbmeta_cloudnode model_node ;
					auto field = FieldExtractor{ model_file, model_node } ;

					auto ck = global_db()
						.Query( model_file )
						.Join( model_node, field( model_file.service ) == field( model_node.user ) )
						.Where(
							field( model_file.id ) == chunk.id &&
							field( model_file.bytes ) == chunk.bytes &&
							field( model_file.beg ) == chunk.beg &&
							field( model_file.end ) == chunk.end ).ToVector() ;

					bool ok = false ;
					for ( auto&& row : ck )
					{
						auto beg = stoull( std::get< 3 >( row ).Value() ) ;
						auto end = stoull( std::get< 4 >( row ).Value() ) ;
						auto number = stoull( std::get< 6 >( row ).Value() ) ;

						auto user = std::get< 7 >( row ).Value() ;
						auto pawd = std::get< 8 >( row ).Value() ;
						auto pop3 = std::get< 10 >( row ).Value() ;

						if ( email_recv(
							pop3.c_str(),
							user.c_str(),
							pawd.c_str(),
							number,
							file.c_str(),
							0, end - beg, beg ) == 200 )
						{
							ok = true ;
							break ;
						}
					}

					if ( !ok )
					{
						throw 1 ;
					}
				}

				catch ( ... )
				{
					result->set_value( false ) ;
					return ;
				}

				result->set_value( true ) ;
			} ) ;
		}
		
		pmr.reduce( [ &ret ]( auto ok )
		{
			if ( ok == false )
			{
				ret = false ;
			}
		} ) ;
	}

	catch ( ... )
	{
		ret = false ;
	}

	if ( ret == false )
	{
		remove( file.c_str() ) ;
	}

	return ret ;
}

bool global_cloudfile_download( const string& id, const string& file )
{
	if ( global_cloudfile_download_single_chunk( id, file ) )
	{
		return true ;
	}

	if ( global_cloudfile_download_multi_chunks( id, file ) )
	{
		return true ;
	}

	return false ;
}

//////////////////////////////////////////////////////////////////////////
