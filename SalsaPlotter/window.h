#ifndef	WINDOW_H
#define	WINDOW_H	1
#include <GhGui.h>
#include "myApp.h"
#include "ghGuiCommon.h"

namespace	GH
{
class	Window	:	public	MainWindow
{
	Q_OBJECT

public:
	Window( MyApp* app  = 0 );
public slots:
	void	doPlot();
	void	enableYParams( const bool& tf );
private:
	MyApp		*_myApp;

//	QWidget	*centralWidget, *topWidget, *btnWidget, *botWidget;

	QWidget		*_centralWidget;
	QGridLayout	*_centralLayout;
};
}	//	GH namespace
#endif	//	WINDOW_H
