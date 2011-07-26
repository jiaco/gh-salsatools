#include "window.h"

namespace	GH
{
const	char*	WINDOW_TITLE = "SalsaTabler";
//Open in Excel
//Delete Column(s)

	Window::Window( MyApp* app )
	: MainWindow( app )
{
	_myApp = app;
	setWindowTitle( tr( WINDOW_TITLE ) );

	//createParamView();

	//
	//	CentralWidget houses a QScrollArea with a TableView
	//
	QWidget		*cw = new QWidget( this );
	QVBoxLayout	*lay = new QVBoxLayout;
	QScrollArea	*scrollArea = new QScrollArea( this );

	QToolBar	*toolBar = new QToolBar( this );
	toolBar->setObjectName( "toolbar" );
	addToolBar( toolBar );

	setupDock();

	//
	//	actions that are in the toolbar
	//
	//ActionView::AddListener( this, "OpenExcel", this, SLOT( callOpenInExcel() ) );
	ActionView::AddListener( this, "OpenExcel", _myApp, SLOT( openInExcel() ) );

	toolBar->addAction( ActionView::Action( this, "OpenExcel" ) );
	toolBar->addAction( ActionView::Action( this, "Doit" ) );

	//
	//	actions that are triggered by right-click on the table
	//
	QAction	*_actDeleteColumn = new QAction( tr( "Delete Column(s)" ), this );
	_actDeleteColumn->setToolTip( "Delete Selected Columns" );

	connect( _actDeleteColumn, SIGNAL( triggered() ),
	 this, SLOT( callDeleteColumn() ) );

	gridWidget = new GridWidget( this );
	gridWidget->setData( _myApp->tableHeader(), _myApp->tableData() );

	showMessage( _myApp->tableHeader().join( "," ), Error::Info );

/*
	tableView = new TableView( this );
	tableView->setModel( app->getModel() );
	tableView->setAlternatingRowColors( true );
	tableView->setContextMenuPolicy( Qt::ActionsContextMenu );
	tableView->addAction( _actDeleteColumn );
	scrollArea->setWidget( tableView );
*/
	gridWidget->tableView()->addAction( _actDeleteColumn );
	gridWidget->tableView()->setAlternatingRowColors( true );
	gridWidget->tableView()->setContextMenuPolicy( Qt::ActionsContextMenu );
	scrollArea->setWidget( gridWidget );
	scrollArea->setWidgetResizable( true );
	lay->addWidget( scrollArea );
	cw->setLayout( lay );
	setCentralWidget( cw );

	connect( _myApp, SIGNAL( newTableData() ), this, SLOT( updateTableData() ) );
}
void	Window::updateTableData()
{
	gridWidget->setData( _myApp->tableHeader(), _myApp->tableData() );
}
/*
void	Window::callOpenInExcel()
{
	Tfp	tfp;
	tfp.open( "XXXXXX.xls" );
	_myApp->dumpToFile( tfp );
	tfp.close();
	OpenWith( tfp.getPath() );
}
*/
void	Window::callDeleteColumn()
{
	QList<int> targets;
	//QModelIndexList idxList = tableView->selectionModel()->selectedIndexes();
	QModelIndexList idxList = gridWidget->tableView()->selectionModel()->selectedIndexes();
	for( int i = 0; i < idxList.size(); ++i ) {
		if( !targets.contains( idxList[ i ].column() ) ) {
			targets << idxList[ i ].column();
		}
	}
	if( targets.size() == 0 ) {
		showMessage( Warning( "Must select columns before delete" ) );
		return;
	}
	_myApp->deleteColumn( targets );
}
void	Window::setupDock()
{
	dockWidget = new QDockWidget( "Program Control", this );
	dockWidget->setObjectName( "ControlDock" );

	tabWidget = new QTabWidget( this );

	{	
		QWidget		*bedWidget = new QWidget( this );
		QGridLayout	*lay = new QGridLayout;

		bedWidget->setLayout( lay );

		pui( lay, QString( "BedInput,WigInput,Output" ).split( "," ), 0 );

		FileView::Configure( this, "BedInput", FileView::Open,
			"Select a BED file containing primary feature set", BED_SUFFIX );
		FileView::Configure( this, "WigInput", FileView::OpenMulti,
			"Select WIG file(s) containing data to analyze", WIG_SUFFIX );
		FileView::Configure( this, "Output", FileView::Save,
			"Select Output file [use .xls for 'Open in Excel' to work]"  );
	
		tabWidget->addTab( bedWidget, "Input/Output" );

	}

	{
		QWidget		*wigWidget = new QWidget( this );
		QScrollArea	*wigScrollArea = new QScrollArea( this );
		wigScrollArea->setWidget( wigWidget );
		wigScrollArea->setWidgetResizable( true );
		QGridLayout	*lay = new QGridLayout;
		int	row = 0;
	
		wigWidget->setLayout( lay );
	
		paramView( "WigLabel" )->addToGrid( lay, row, 0 );
		++row;
	
		QGroupBox	*gbZones = new QGroupBox( this );
		QGridLayout	*gbZlay = new QGridLayout;
	
		gbZones->setLayout( gbZlay );
	
		setupGroupBoxView( "UpZone", gbZlay, 0, 0, QStringList() << "UpZone_UpDist" << "UpZone_DnDist" );
		setupGroupBoxView( "Zone", gbZlay, 1, 0, QStringList() << "Zone_UpDist" << "Zone_DnDist" );
		setupGroupBoxView( "DnZone", gbZlay, 2, 0, QStringList() << "DnZone_UpDist" << "DnZone_DnDist" );
		setupGroupBoxView( "Exon", gbZlay, 3, 0, QStringList() << "Exon_UpDist" << "Exon_DnDist" );
		setupGroupBoxView( "Intron", gbZlay, 4, 0, QStringList() << "Intron_UpDist" << "Intron_DnDist" );
	
		lay->addWidget( gbZones, row, 0, 1, -1 );
		++row;
	
		row = pui( lay, QStringList() <<
			"MeanOnCount" << "MeanOnLength" << "Median" << "Variance" << "Count", row );
	
		row = pui( lay, QStringList() << "Doit", row );
		ActionView::AddListener( this, "Doit", _myApp, SLOT( doit() ) );
	
		tabWidget->addTab( wigScrollArea, "Parameters" );
	}

	{
		QWidget		*statWidget = new QWidget( this );
		QGridLayout	*lay = new QGridLayout;
	
		statWidget->setLayout( lay );
		initLogWidget();
		lay->addWidget( logWidget() );
	
		tabWidget->addTab( statWidget, "Statistics" );
	
		// to see things in the LogWidget use this:
		//showMessage( "Starting up", Error::Info );
	}
	dockWidget->setWidget( tabWidget );
	addDockWidget( Qt::LeftDockWidgetArea, dockWidget );
}
void	Window::setupGroupBoxView( const QString& groupBoxName,
	 QGridLayout* mainLayout, const int& mainRow, const int& mainCol,
	 const QStringList& innerNames )
{
	QGridLayout *lay = new QGridLayout;
	int r = 0;
	int c = 0;
	for( int i = 0; i < innerNames.size(); ++i ) {
		paramView( innerNames.at( i ) )->addToGrid( lay, r, c );
		c += 2;
		if( c > 2 ) {
			++r;
			c = 0;
		}
	}
	BooleanView::AddAsGroupBox( this, groupBoxName, mainLayout, mainRow, mainCol, lay );
}
}	//	GH namespace
