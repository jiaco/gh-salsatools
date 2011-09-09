#include "window.h"

namespace	GH
{
	Window::Window( MyApp* app )
	: MainWindow( app )
{
	_myApp = app;
	setWindowTitle( "myProgram" );

	// now in ghMainWindow
	//createParamView();

	_centralLayout = new QGridLayout;
	_centralWidget = new QWidget( this );

	_centralWidget->setLayout( _centralLayout );

	pui( _centralLayout, 0 );
	ActionView::AddListener( this, "Doit", _myApp, SLOT( doit() ) );
	setCentralWidget( _centralWidget );
}
}	//	GH namespace
