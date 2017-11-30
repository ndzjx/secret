
#include <QApplication>
#include "ecloud.h"
#include "CUploadWidget.h"

//////////////////////////////////////////////////////////////////////////

// 计划: 更新云端信息
void plan_cloudnodes_update()
{
	auto timer = std::make_shared<boost::asio::deadline_timer>( global_pc().get_ios(), boost::posix_time::seconds( 5 ) ) ;

	auto fina = shared_ptr<void>( nullptr, [ timer ](void*)
	{
		if ( global_pc().get_ios().stopped() == false )
		{
			plan_cloudnodes_update() ;
		}
	} ) ;
	
	timer->async_wait( [ timer, fina ]( boost::system::error_code ec )
	{
		if ( ec == 0 )
		{
			global_cloudnodes_update( fina ) ;
		}
	} ) ;
}

int main( int argc, char *argv[] )
{
	ParallelCore pc_task ;
	global_pc( &pc_task ) ;
	
	ORMapper db( ( boost::filesystem::system_complete( __argv[ 0 ] ).remove_filename() /= ( "ecloud.db" ) ).generic_string() ) ;
	global_db( &db ) ;

	plan_cloudnodes_update() ;

	QApplication app( argc, argv ) ;
    CUploadWidget w ;
    w.show() ;
	app.exec() ;
	
	global_pc().stop() ;
	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
