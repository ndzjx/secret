
#include <QApplication>
#include "CUploadWidget.h"

#include <mod_meta.h>

int demo_download()
{
	using namespace secret ;

	service_cloud_t			g_cloud		; // 邮箱元信息索引
	service_index_t			g_index		; // 文件元信息索引
	ParallelCore			g_pc		; // 并行调度器

	// 任务 - 生成邮箱元信息索引
	{
		ParallelMapReduce<service_meta> pmr ;

		pmr.map( g_pc, []( auto result )
		{
			service_meta info ;
			info.smtp = "smtp.126.com" ;
			info.pop3 = "pop.126.com" ;
			info.user = "testhackpro@126.com" ;
			info.pawd = "123abc" ;
			service_update( info ) ;
			result->set_value( info ) ;
		} ) ;

		pmr.reduce( [&g_cloud]( auto&& info )
		{
			g_cloud.emplace( info.user, info ) ;
		} ) ;
	}

	// 任务 - 生成文件元信息索引
	{
		ParallelMapReduce<service_index_meta_t> pmr ;

		for ( auto&& node : g_cloud )
		{
			service_callback_subject( node.second, [ &pmr, &g_pc ]( auto user )
			{
				pmr.map( g_pc, [ user ]( auto result )
				{
					result->set_value( user() ) ;
				} ) ;
			} ) ;
		}

		pmr.reduce( [ &g_index ]( auto&& meta )
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
			
			g_index[ fm.id ].push_back( meta ) ;
		} ) ;
	}
	
	if ( true )
	{
		// 下载这个文件
		const char* id = "FA8249C2124C9F96DCBED0DDAD9040E3" ;
		const char* file = "g:/hehe.png" ;
		file_from_single_chunk( g_cloud, g_index, id, file ) ;
	}

	return 0 ;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CUploadWidget w;
    w.show();

	demo_download() ;

    return app.exec();
}
