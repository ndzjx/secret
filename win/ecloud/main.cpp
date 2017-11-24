
#include <QApplication>
#include "CUploadWidget.h"

//////////////////////////////////////////////////////////////////////////

using namespace secret ;

service_cloud_t* g_cloud = nullptr ;
service_cloud_t& global_cloud()
{
	return *g_cloud ;
}

service_index_t* g_index = nullptr ;
service_index_t& global_index()
{
	return *g_index ;
}

ParallelCore* g_pc = nullptr ;
ParallelCore& global_pc()
{
	return *g_pc ;
}

void global_index_update()
{
	ParallelMapReduce<service_index_meta_t> pmr ;

	for ( auto&& node : global_cloud() )
	{
		auto news = service_update( node.second ) ;
		if ( news == 0 )
		{
			continue ;
		}

		service_callback_subject( node.second, [ &pmr ]( auto user )
		{
			pmr.map( global_pc(), [ user ]( auto result )
			{
				result->set_value( user() ) ;
			} ) ;
		}, node.second.mails - news + 1, node.second.mails ) ;
	}

	pmr.reduce( []( auto&& meta )
	{
		auto&& fm = meta.second ;
		if ( fm.id.empty() )
		{
			return ;
		}

		if ( fm.bytes == 0 || fm.end == 0 )
		{
			return ;
		}
			
		global_index()[ fm.id ].push_back( meta ) ;
	} ) ;
}

void global_init()
{
	// 任务 - 生成邮箱云索引
	{
		ParallelMapReduce<service_meta> pmr ;

		pmr.map( global_pc(), []( auto result )
		{
			service_meta info ;
			info.smtp = "smtp.126.com" ;
			info.pop3 = "pop.126.com" ;
			info.user = "testhackpro@126.com" ;
			info.pawd = "123abc" ;
			result->set_value( info ) ;
		} ) ;

		pmr.reduce( []( auto&& info )
		{
			global_cloud().emplace( info.user, info ) ;
		} ) ;
	}

	// 任务 - 生成文件元信息索引
	global_index_update() ;
}

//////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
	service_cloud_t cloud ;
	service_index_t index ;
	ParallelCore pc ;
	g_cloud = &cloud ;
	g_index = &index ;
	g_pc = &pc ;
	global_init() ;
	
	QApplication app( argc, argv ) ;

    CUploadWidget w ;
    w.show() ;

    return app.exec() ;
}

//////////////////////////////////////////////////////////////////////////
