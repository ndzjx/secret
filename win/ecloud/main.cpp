
#include <QApplication>
#include "ecloud.h"
#include "CUploadWidget.h"

//////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
	ParallelCore pc ;
	global_pc( &pc ) ;

	ORMapper db( "g:/sample.db" ) ;
	global_db( &db ) ;

	global_update_cloud() ;
	
	QApplication app( argc, argv ) ;

    CUploadWidget w ;
    w.show() ;

    return app.exec() ;
}

//////////////////////////////////////////////////////////////////////////
