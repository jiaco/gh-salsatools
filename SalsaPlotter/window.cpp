#include "window.h"
namespace	GH
{
	Window::Window( MyApp* app )
	 : MainWindow( app )
{
	_myApp = app;
	setWindowTitle( "Solexa Plotter" );

//	createParamView();

	QStringList	topPList, botPList;

	QString suffixInput = WIG_SUFFIX;
	QString messageInput = tr( "Select input wig file(s)" );
	QString descriptionInput =
	 tr( "Input data files (such as wig-format vstep data)" );

	QString suffixFeature = BED_SUFFIX;
	QString messageFeature = tr( "Select feature bed file(s)" );
	QString descriptionFeature =
	 tr( "Feature data files (such as bed-format domains/peaks data)" );

	_centralWidget = new QWidget( this );
	_centralLayout = new QGridLayout;
	_centralWidget->setLayout( _centralLayout );

	topPList << "InputFile" << "FeatureFile"
		<< "DWS" << "SWS";

	botPList << "ImageTitle" << "ImageWidth" << "ImageHeight"
		 << "LineColor"
		 << "YMin" << "YMax";

	int row = 0;
	row = pui( _centralLayout, topPList, row );
	paramView( "PreCalc" )->addToGrid( _centralLayout, row, 0 );
	paramView( "Plot" )->addToGrid( _centralLayout, row, 1 );
	++row;
	row = pui( _centralLayout, botPList, row ); 

	ActionView::AddListener( this, "PreCalc", app, SLOT( doPreCalc() ) );
	ActionView::AddListener( this, "Plot", this, SLOT( doPlot() ) );

	connect( app, SIGNAL( preCalcFinished(bool) ),
	 this, SLOT( enableYParams(bool) ) );
	
	FileView::Configure( this, "InputFile",
	 FileView::OpenMulti, messageInput, suffixInput );
	FileView::Configure( this, "FeatureFile",
	 FileView::OpenMulti, messageFeature, suffixFeature );
	FileView::Configure( this, "ImageFile",
	 FileView::Save, "Select a PNG output path", "png" );
	
	setCentralWidget( _centralWidget );
/*
	FileParam* fpIn = FindParam<FileParam>( this, "InputFile" );
	fpIn->configure( FileParam::OpenMulti, messageInput, suffixInput );

	FindParam<FileParam>( this, "FeatureFile" )->configure(
		FileParam::OpenMulti, messageFeature, suffixFeature );
*/
/*
	//fpIn->addButton( "Load", app, SLOT( loadInput() ) );

	FileParam* fpFe = FindParam<FileParam>( this, "FeatureFile" );
	fpFe->configure( FileParam::OpenMulti, messageFeature, suffixFeature );
	//fpFe->addButton( "Load", app, SLOT( loadFeature() ) );
*/
	enableYParams( false );
}
void	Window::enableYParams( const bool& tf )
{
	param( "YMin" )->setEnabled( tf );
	param( "YMax" )->setEnabled( tf );
}
void	Window::doPlot()
{
	//if( app->runBounds() ) {

		ImageWidget* iw;
		QImage *_image = _myApp->doPlot();
		if( _myApp->hasError() ) {
			showMessage( _myApp->error() );
		} else {
			iw = new ImageWidget( _image, GetParam<FileView>( this, "ImageFile" ), this );
			iw->show();
		}

	//}
}
}	//	GH namespace
