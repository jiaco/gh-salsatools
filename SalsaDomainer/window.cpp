#include "window.h"
#include <QDebug>

namespace	GH
{
	Window::Window( MyApp* app )
	: MainWindow( app )
{
	_myApp = app;
	setWindowTitle( "Salsa Domainer" );
	//createParamView();

	_widgetCentral = new QWidget( this );
	_tabWidget = new QTabWidget( this );

	QGridLayout *layoutInput = new QGridLayout;
	_widgetInput = new QWidget( this );
	_widgetInput->setLayout( layoutInput );
	int row = 0;
	row = pui( layoutInput,
		 QStringList()
		 	<< "FileInput"
			 << "LoadInput"
			<< "Resolution",
		 row );
	FileView::Configure( this, "FileInput",
		FileView::OpenMulti, "Select input wig file(s)", WIG_SUFFIX );
	ActionView::AddListener( this, "LoadInput",
	 _myApp, SLOT( loadInput() ) );
	

	QGridLayout *layoutDomains = new QGridLayout;
	_widgetDomains = new QWidget( this );
	_widgetDomains->setLayout( layoutDomains );
	row = 0;
	row = pui( layoutDomains,
		 QStringList()
			<< "FileDomains"
			 << "LoadDomains"
		 	<< "MinSeedValue"
		 	<< "MinSeedNumProbes"
		 	<< "MinSeedScore"
		 	<< "MinCoverage"
		 	<< "MergeDistance"
		 	<< "MinZoneSize"
		 	<< "MinZoneScore"
		 	<< "ExtendWidth"
		 	<< "ExtendPctOver"
		 	<< "RunAllDomain"
			<< "Domains",	// the Action

		row );
	FileView::Configure( this, "FileDomains",
		FileView::OpenMulti, "Select bed domain file(s)", BED_SUFFIX );
	ActionView::AddListener( this, "LoadDomains",
	 _myApp, SLOT( loadDomains() ) );
		
	QGridLayout *layoutPeaks = new QGridLayout;
	_widgetPeaks = new QWidget( this );
	_widgetPeaks->setLayout( layoutPeaks );

	row = 0;
	row = pui( layoutPeaks,
		QStringList()
			<< "FilePeaks"
			 << "LoadPeaks"
			<< "PeakMinNumProbes"
			<< "PeakMinPctAvg"
			<< "PeakMinPctOver"
			<< "PeakMergeDistance"
			<< "RunAllPeak"
			<< "Peaks",	// the Action
		row );
	FileView::Configure( this, "FilePeaks",
		FileView::OpenMulti, "Select bed peak file(s)", BED_SUFFIX );
	ActionView::AddListener( this, "LoadPeaks",
	 _myApp, SLOT( loadPeaks() ) );
/*
	_widgetInput = pui( this, this->app, QStringList()
	 << "FileInput"
	 << "Resolution"
	 );
	FileParam::Configure( this, "FileInput", FileParam::OpenMulti,
		"Select input wig file(s)", WIG_SUFFIX );
	FileParam::AddButton( this, "FileInput", "Load",
	 	this->app, SLOT( loadInput() ) );
	_widgetDomains = PUI( this, this->app,
		 QStringList()
		 << "FileDomains"
		 << "MinSeedValue"
		 << "MinSeedNumProbes"
		 << "MinSeedScore"
		 << "MinCoverage"
		 << "MergeDistance"
		 << "MinZoneSize"
		 << "MinZoneScore"
		 << "ExtendWidth"
		 << "ExtendPctOver"
		 << "RunAllDomain"
	 );
	FileParam::Configure( this, "FileDomains", FileParam::OpenMulti,
		"Select bed domain file(s)", BED_SUFFIX );
	FileParam::AddButton( this, "FileDomains", "Load",
	 	this->app, SLOT( loadDomains() ) );
	// add buttons RunAllDomain, domainFind

	_widgetPeaks = PUI( this, this->app,
		 QStringList()
		<< "FilePeaks"
		<< "PeakMinNumProbes"
		<< "PeakMinPctAvg"
		<< "PeakMinPctOver"
		<< "PeakMergeDistance"
		<< "RunAllPeak"
	);
	FileParam::Configure( this, "FilePeaks", FileParam::OpenMulti,
		"Select bed peak file(s)", BED_SUFFIX );
	FileParam::AddButton( this, "FilePeaks", "Load",
	 	this->app, SLOT( loadPeaks() ) );

	logWidget()->setMinimumSize( 600, 600 );

	connect( app, SIGNAL( log(QString) ),
	 logWidget(), SLOT( log(QString) ) );
	connect( this, SIGNAL( log(QString) ),
	 logWidget(), SLOT( log(QString) ) );
*/

	_tabWidget->addTab( _widgetInput, "Input" );
	_tabWidget->addTab( _widgetDomains, "Domains" );
	_tabWidget->addTab( _widgetPeaks, "Peaks" );
	//_tabWidget->addTab( logWidget(), "Log" );

	QGridLayout	*layout = new QGridLayout;
	_widgetCentral->setLayout( layout );

	paramView( "CurrentSid" )->addToGrid( layout, 0, 0 );
	paramView( "MaxValue" )->addToGrid( layout, 1, 0 );
	layout->addWidget( _tabWidget, 2, 0, 1, -1 );

/*
	layout->addWidget(
	 PUI( this, this->app, "CurrentSid" ), 0, 0
	);
	layout->addWidget(
	 PUI( this, this->app, "MaxValue" ), 1, 0
	);
	layout->addWidget( _tabWidget, 2, 0 );
*/

/* this would have been cool, maybe it is
	ComboParam::AddListener( this, "CurrentSid",
	 this, SLOT( updateSceneWidget(QString) ) );
*/
	ConnectToSlot<ChoiceView>( this, "CurrentSid", SIGNAL( activated(QString) ),
		this, SLOT( activateSceneWidget(QString) ) );
	ConnectToSignal<ChoiceView>( this, "CurrentSid", _myApp, SIGNAL( inputChanged(QStringList) ),
		SLOT( setChoices(QStringList) ) );
		
/*
	{
		ComboParam *p;
		p = _widgetCentral->findChild<ComboParam *>( "CurrentSid" );
		if( p && p->inherits( "GH::ComboParam" ) ) {
			connect( p, SIGNAL( activated(QString) ),
			 this, SLOT( activateSceneWidget(QString) ) );
			connect( this->app, SIGNAL( inputChanged(QStringList) ),
			 p, SLOT( setTexts(QStringList) ) );
		}

	}
*/
	ActionView::AddListener( this, "Domains", _myApp, SLOT( doDomainFind() ) );
	ActionView::AddListener( this, "Peaks", _myApp, SLOT( doPeakFind() ) );
/*
	AddButton( _widgetDomains, "Domains",
		this->app, SLOT( doDomainFind() ) );
	AddButton( _widgetPeaks, "Peaks",
		this->app, SLOT( doPeakFind() ) );
*/


	setCentralWidget( _widgetCentral );

	// more connections...

	connect( app, SIGNAL( domainsUpdated(QString) ),
	 this, SLOT( dispatchUpdate(QString) ) );
	connect( app, SIGNAL( peaksUpdated(QString) ),
	 this, SLOT( dispatchUpdate(QString) ) );

	
}
void	Window::activateSceneWidget( const QString& sid )
{
	if( !swmap.contains( sid ) ) {
		SceneWidget *p = new SceneWidget( sid, _myApp, this );
		connect( p, SIGNAL( message( QString, int ) ),
		 this, SLOT( showMessage( QString, int ) ) );
// need a set of controls
// for these so that new scene widgets are created properly
		// width, height, margin, resolution, ymaxvalue
		//p->setGraphicParams( 600, 600, 10, 50, 10 ); 
		//p->attachInput( app->input( sid ) );
		swmap.insert( sid, p );
	}
	swmap[ sid ]->show();
	swmap[ sid ]->raise();
}
void	Window::dispatchUpdate( const QString& sid )
{
	if( swmap.contains( sid ) ) {
		swmap[ sid ]->redraw();
	}
}
}	//	GH namespace
