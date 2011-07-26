#include <GhCore.h>
#include "myApp.h"
#include "window.h"

using namespace	GH;

int	main( int argc, char** argv )
{
	Q_INIT_RESOURCE( application );

	QApplication	qapp( argc, argv );
	MyApp		app( argc, argv );
	Window		*window;

	if( app.isGui() ) {
		GH_CORE_SETUP( app.arg0() );

		window = new Window( &app );
		window->show();
		return( qapp.exec() );
	} else {
		app.doit();
		return( 0 );
	}
}
