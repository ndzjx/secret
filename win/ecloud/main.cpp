
#include <QApplication>
#include <QSplashScreen>
#include "ecloud.h"
#include "MainForm.h"
#include "CBrowserWidget.h"

//////////////////////////////////////////////////////////////////////////

// 计划: 更新云端信息
template<class T>
void plan_cloudnodes_update( T&& sig )
{
	auto timer = std::make_shared<boost::asio::deadline_timer>( global_pc().get_ios(), boost::posix_time::seconds( 3 ) ) ;

	auto fina = shared_ptr<uint64_t>( new uint64_t( 0 ), [ timer, &sig ]( uint64_t* pNews )
	{
		if ( pNews && *pNews )
		{
			sig() ;
		}

		delete pNews ;
		pNews = nullptr ;

		if ( global_pc().get_ios().stopped() == false )
		{
			plan_cloudnodes_update( sig ) ;
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
	QApplication app( argc, argv ) ;
    auto pLogo = new QSplashScreen( QPixmap( ":/res/logo.png" ) ) ;
    pLogo->show() ;
    app.processEvents() ;
	
    pLogo->showMessage( "Initialization Start Engines ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
    ParallelCore pc_task( 6 ) ;
    global_pc( &pc_task ) ;
    boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
	
    pLogo->showMessage( "Initialization SQLite ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
    ORMapper db( ( boost::filesystem::system_complete( __argv[ 0 ] ).remove_filename() /= ( "ecloud.db" ) ).generic_string() ) ;
    global_db( &db ) ;
    boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;

    pLogo->showMessage( "Ready Widgets ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	MainForm wMain ;
    boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
	
	pLogo->showMessage( "Initialization Tasks ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	boost::signals2::signal<void()> sig ;
	sig.connect( std::bind( &CBrowserWidget::update, wMain.m_pBrowserWidget ) ) ;
    plan_cloudnodes_update( sig ) ;
    boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;

    pLogo->showMessage( "Welcome!", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
    boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
    delete pLogo ;

    wMain.show() ;
	app.exec() ;
	
    global_pc().stop() ;
	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
