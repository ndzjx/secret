
#include <QApplication>
#include "ecloud.h"
#include "CUploadWidget.h"

//////////////////////////////////////////////////////////////////////////

// 计划: 更新云端信息
void plan_update_cloud()
{
	auto timer = std::make_shared<boost::asio::deadline_timer>( global_pc().get_ios(), boost::posix_time::seconds( 2 ) ) ;

	auto fina = shared_ptr<void>( nullptr, [ timer ](void*)
	{
		if ( global_pc().get_ios().stopped() == false )
		{
			plan_update_cloud() ;
		}
	} ) ;
	
	timer->async_wait( [ timer, fina ]( boost::system::error_code ec )
	{
		if ( ec == 0 )
		{
			global_update_cloud( fina ) ;
		}
	} ) ;
}

int main( int argc, char *argv[] )
{
	ParallelCore pc_task ;
	global_pc( &pc_task ) ;
	
	ORMapper db( ( boost::filesystem::system_complete( __argv[ 0 ] ).remove_filename() /= ( "ecloud.db" ) ).generic_string() ) ;
	global_db( &db ) ;

	plan_update_cloud() ;

	QApplication app( argc, argv ) ;
    CUploadWidget w ;
    w.show() ;
	app.exec() ;
	
	global_pc().stop() ;
	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
