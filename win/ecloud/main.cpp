
#include <QApplication>
#include "CUploadWidget.h"

#include <mod_meta.h>

int demo_download()
{
	using namespace secret ;

	service_cloud_t			g_cloud		; // ����Ԫ��Ϣ����
	service_index_t			g_index		; // �ļ�Ԫ��Ϣ����
	ParallelCore			g_pc		; // ���е�����

	// ���� - ��������Ԫ��Ϣ����
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

	// ���� - �����ļ�Ԫ��Ϣ����
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
		// ��������ļ�
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
