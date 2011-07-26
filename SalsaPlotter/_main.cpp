#include <GhCore.h>
#include "myApp.h"
#include "window.h"

using namespace	GH;

int	main( int argc, char** argv )
{
	QApplication	qapp( argc, argv );
	MyApp		app( argc, argv );
	Window		*window;

	if( app.isGui() ) {
		QCoreApplication::setOrganizationName( GH_ORG_NAME );
		QCoreApplication::setOrganizationDomain( GH_ORG_DOMAIN );
		QCoreApplication::setApplicationName( app.arg0() );
		window = new Window( &app );
		//window->syncWithApp( &app );
		window->show();
		return( qapp.exec() );
	} else {
		app.doit();
		return( 0 );
	}
}
