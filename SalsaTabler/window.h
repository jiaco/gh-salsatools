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

public slots:
	void	callDeleteColumn();
	void	updateTableData();
	//void	callOpenInExcel();

private:
	void		setupDock();

	void	setupGroupBoxView( const QString& groupBoxName,
		 QGridLayout * mainLayout, const int& mainRow,
		 const int& mainCol, const QStringList& innerNames );
	//
	//	Main interface components
	//
	//TableView	*tableView;
	GridWidget	*gridWidget;
	QDockWidget	*dockWidget;
	QTabWidget	*tabWidget;

	MyApp	*_myApp;
};

}	//	GH namespace
#endif	//	WINDOW_H
