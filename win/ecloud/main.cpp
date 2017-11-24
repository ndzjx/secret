
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

void global_init()
{
	ParallelMapReduce<service_meta> pmr ;

	pmr.map( global_pc(), []( auto result )
	{
		service_meta info ;
		info.smtp = "smtp.126.com" ;
		info.pop3 = "pop.126.com" ;
		info.user = "testhackpro@126.com" ;
		info.pawd = "123abc" ;
		service_update( info ) ;
		result->set_value( info ) ;
	} ) ;

	pmr.reduce( []( auto&& info )
	{
		global_cloud().emplace( info.user, info ) ;
	} ) ;
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	service_cloud_t cloud ;
	service_index_t index ;
	ParallelCore pc ;
	g_cloud = &cloud ;
	g_index = &index ;
	g_pc = &pc ;
	global_init() ;

    QApplication app(argc, argv);

    CUploadWidget w;
    w.show();

    return app.exec();
}

//////////////////////////////////////////////////////////////////////////
