
#include "ecloud.h"

//////////////////////////////////////////////////////////////////////////

ParallelCore& global_pc( ParallelCore* pc = nullptr )
{
	static ParallelCore* g_pc = nullptr ;
	if ( g_pc == nullptr && pc != nullptr )
	{
		g_pc = pc ;
	}

	return *g_pc ;
}

ORMapper& global_db( ORMapper* db = nullptr )
{
	static ORMapper* g_db = nullptr ;
	if ( g_db == nullptr && db != nullptr )
	{
		g_db = db ;
	}

	return *g_db ;
}

//////////////////////////////////////////////////////////////////////////

void global_update_cloud()
{
	auto task_insert_index = []( service_meta service, int64_t i )
	{
		try
		{
			auto subject = email_subject( service.pop3.c_str(), service.user.c_str(), service.pawd.c_str(), i ) ;
			
			file_meta fm ;
			meta_from_json( fm, subject.c_str() ) ;

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
				// 检查节点更新
				auto service = node.to_meta() ;
				auto news = service_update( service ) ;
				if ( news == 0 )
				{
					continue ;
				}

				// 更新数据库中节点数据
				node.from_meta( service ) ;
				global_db().Update( node ) ;

				// 更新节点文件索引
				for ( int64_t i = service.mails - news + 1 ; i <= service.mails ; ++i )
				{
					global_pc().post( std::bind( task_insert_index, service, i ) ) ;
				}
			}
		}

		catch( ... )
		{

		}
	} ;

	global_pc().post( task_check_update ) ;
}

//////////////////////////////////////////////////////////////////////////
