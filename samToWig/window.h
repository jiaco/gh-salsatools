#ifndef	WINDOW_H
#define	WINDOW_H	1
#include <GhCore.h>
#include <GhGui.h>
#include "myApp.h"

namespace	GH
{

class	Window	:	public	MainWindow
{
	Q_OBJECT

public:
	Window( MyApp* app = 0 );

private:
	MyApp	*_myApp;

	QWidget		*_centralWidget;
	QGridLayout	*_centralLayout;
};

}	//	GH namespace
#endif	//	WINDOW_H
