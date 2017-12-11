
#include "echat.h"

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

vector<dbmeta_cloudnode> global_cloudnodes_all()
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

vector<dbmeta_cloudnode> global_cloudnodes_sender()
{
	vector<dbmeta_cloudnode> result ;
	for ( auto&& node : global_cloudnodes_all() )
	{
		if ( node.smtp.size() > 3 )
		{
			result.emplace_back( node ) ;
		}
	}

	return result ;
}

vector<dbmeta_cloudnode> global_cloudnodes_recver()
{
	vector<dbmeta_cloudnode> result ;
	for ( auto&& node : global_cloudnodes_all() )
	{
		if ( node.pop3.size() > 3 )
		{
			result.emplace_back( node ) ;
		}
	}

	return result ;
}

string global_cloudnodes_dist()
{
	string dist ;
	for ( auto&& dbnode : global_cloudnodes_recver() )
	{
		dist += dbnode.user ;
		dist += ',' ;
	}

	return dist ;
}

//////////////////////////////////////////////////////////////////////////

void global_cloudnodes_update( msgsig_t& sig, std::shared_ptr<void> fina )
{
	auto task_finally = [ fina ]( auto ptr_service, auto ptr_news )
	{
		auto&& service = *ptr_service ;
		auto&& news = *ptr_news ;

		dbmeta_cloudnode node ;
		service.mails += news ;
		node.from_meta( service ) ;
		global_cloudnode_fix( node ) ;
	} ;

	auto task_insert_index = [ task_finally, &sig ]( auto self, auto ptr_service, auto ptr_news )
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

		if ( fm.id.empty() )
		{
			return ;
		}

		if ( fm.id.substr( 0, 5 ) != ".chat" )
		{
			return ;
		}

		// Notify update
		sig( fm.id, service, fm, i ) ;
	} ;

	auto task_main = [ task_insert_index ]
	{
		try
		{
			for ( auto&& node : global_cloudnodes_recver() )
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

		dbmeta_cloudfile model ;
		auto field = FieldExtractor{ model } ;
		global_db().Delete( dbmeta_cloudfile{}, field( model.service ) == dbnode.user ) ;
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

map< pair<string, string>, string > global_clouddirs()
{
	map< pair<string, string>, string > dict_dir_tree ;

	try
	{
		dbmeta_cloudfile model ;
		auto field = FieldExtractor{ model } ;

		map< pair<string, string>, int > dict_user_path ;
		map< pair<string, string>, vector<dbmeta_cloudfile> > dict_dir_nodes ;
		
		for ( auto&& dir : global_db()
			.Query( model )
			.Where( field ( model.id ) & std::string( ".dir%" ) )
			.ToVector() )
		{
			vector<string> meta ;
			boost::algorithm::split( meta, dir.id, boost::is_any_of( "?" ) ) ;
			if ( meta.size() != 4 )
			{
				continue ;
			}

			dict_user_path[ make_pair( meta[ 1 ], meta[ 2 ] ) ] ;
		}

		for ( auto&& key : dict_user_path )
		{
			string like = ".dir?" ;
			like += key.first.first ;
			like += "?" ;
			like += key.first.second ;
			like += "?%" ;
			
			auto dirs = global_db()
				.Query( model )
				.Where( field ( model.id ) & like )
				.OrderByDescending( field ( model.id ) )
				.Take( 1 )
				.ToVector() ;
			if ( dirs.empty() )
			{
				continue ;
			}
			
			dict_dir_nodes[ key.first ] = global_db()
				.Query( model )
				.Where( field ( model.id ) & dirs[ 0 ].id )
				.ToVector() ;
		}

		for ( auto&& key : dict_dir_nodes )
		{
			for ( auto&& dir : key.second )
			{
				dbmeta_cloudnode node ;
				if ( !global_cloudnode_get( dir.service, node ) )
				{
					continue ;
				}

				string tree = email_content(
					node.pop3.c_str(),
					node.user.c_str(),
					node.pawd.c_str(),
					stoull( dir.number ) ) ;
				if ( tree.empty() )
				{
					continue ;
				}

				dict_dir_tree[ key.first ] = tree ;
				break ;
			}
		}
	}

	catch ( ... )
	{

	}

	return dict_dir_tree ;
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
