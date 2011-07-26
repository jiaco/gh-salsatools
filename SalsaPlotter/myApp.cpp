#include "myApp.h"

namespace	GH
{

QImage *MakeImage( const QList<Wig>& data )
{
	QImage	*image = new QImage( 600, 600, QImage::Format_ARGB32 );

	qreal	width = 600;
	qreal	height = 600;
	qreal	xrange, yrange;
	qreal	xlo, xhi, ylo, yhi;

	if( data.size() == 0 ) {
		return( image );
	}
	{
	int i = 0;
	xlo = xhi = data.at( i ).beg;
	ylo = yhi = data.at( i ).val;
	for( i = 1; i < data.size(); ++i ) {
		if( data.at( i ).beg < xlo ) {
			xlo = data.at( i ).beg;
		}
		if( data.at( i ).beg > xhi ) {
			xhi = data.at( i ).beg;
		}
		if( data.at( i ).val < ylo ) {
			ylo = data.at( i ).val;
		}
		if( data.at( i ).val > yhi ) {
			yhi = data.at( i ).val;
		}
	}
	} // bad ws
	xrange = xhi - xlo;
	yrange = yhi - ylo;

	Scene	*scene = new Scene();
	QRectF  sceneRect = QRectF( 0, 0, width, height );

	QPen 	blackpen = QPen( QColor( Qt::black ) );
	QPen 	pen = QPen( QColor( Qt::white ) );
	QBrush  brush = QBrush( QColor( Qt::white ) );

	qreal	w = 1;
	qreal	h = 1;

	scene->clear();
	scene->setSceneRect( sceneRect );
	scene->setBackgroundBrush( Qt::red );

	{
	int i = 0;
	qreal lastx = ( data.at( i ).beg / xrange ) * width;
	qreal lasty = ( data.at( i ).val / yrange ) * height;
	lasty = height - lasty;
	for( i = 1; i < data.size(); ++i ) {
		qreal x = ( data.at( i ).beg / xrange ) * width;
		qreal y = ( data.at( i ).val / yrange ) * height;

		y = height - y;

		scene->addRect( x, y, w, h, pen, brush );
		scene->addLine( lastx, lasty, x, y, blackpen );
		lastx = x;
		lasty = y;
	}
	} // end bad ws

	QPainter *painter = new QPainter( image );
	scene->render( painter );
	delete painter;
	return( image );
}
	MyApp::MyApp( int argc, char** argv )
	: CliApp( argc, argv )
{
	cout.open( STDOUT );
	cerr.open( STDERR );

	addParam( "InputFile", "", ParamModel::File );
	addParam( "FeatureFile", "", ParamModel::File );
	addParam( "ImageFile", "", ParamModel::File );

	addParam( "DWS", "100000", ParamModel::Edit );
	addParam( "SWS", "10000", ParamModel::Edit );

	addParam( "ImageTitle", "SolexaPlotter Image", ParamModel::Edit );
	addParam( "ImageWidth", "600", ParamModel::Edit );
	addParam( "ImageHeight", "600", ParamModel::Edit );

	addParam( "LineColor", "", ParamModel::Color );

	addParam( "YMin", "", ParamModel::Edit );
	addParam( "YMax", "", ParamModel::Edit );

	addParam( "PreCalc", "", ParamModel::Action );
	addParam( "Plot", "", ParamModel::Action );

	imager = 0;
	_usedPreCalc = false;

	_isGui = !checkArgs();
	_isGui = true;
}
void	MyApp::doit()
{
/*
	setParam( "InputFile", Scan( args[ "i" ], QStringList() << "wig" ) );
	setParam( "FeatureFile", Scan( args[ "d" ], QStringList() << "bed" ) );
	loadInput();
	loadFeature();
	runBounds();
*/
}
	/*
	 *	validate() is ONLY CALLED by doMethod()
	 */
bool	MyApp::validate()
{
	// if we are going to load a file,
	//  we need to reset the interface for Ymin/Ymax
	//
	_usedPreCalc = false;
	emit( preCalcFinished( false ) );

	_dws = I( param( "DWS" )->value() );
	_sws = I( param( "SWS" )->value() );
	_inputFiles = SL( param( "InputFile" )->value() );
	_featureFiles = SL( param( "FeatureFile" )->value() );

	if( _dws <= 0 || _sws <= 0 || _dws <= _sws ) {
		setCritical( "Unsupported DWS/SWS settings" );
	} else if( _inputFiles.size() == 0 ) {
		setCritical( "No Input File(s) specified" );
	} else if( _featureFiles.size() == 0 ) {
		setCritical( "No Feature File(s) specified" );
	}
	return( errorState() );
}
bool	MyApp::doMethod()
{
	_usedPreCalc = false;

	// doMethod handles its own errors
	// either through self-handling loaders or
	// validate and runBounds which are shown in this method
	//
	if( validate() ) {
		// load errors are shown in the load methods
		if( loadInput() && loadFeature() ) {
			if( !runBounds() ) {
				showError();
			}
		}
	} else {
		// validation errors require calling method to show
		showError();
	}
	return( errorState() );
}
bool	MyApp::doPixels()
{
	_imageWidth = I( param( "ImageWidth" )->value() );
	_imageHeight = I( param( "ImageHeight" )->value() );

	if( imager != 0 ) {
		delete imager;
		imager = 0;
	}
	imager = new Imager( _imageWidth, _imageHeight,
		 60, 60, 60, 60 );

	axes.xinit( ( _dws * -1 ), _dws );
	if( ( xincr = axes.xrange / imager->xspace ) <= 0 ) {
		xincr = 1;
	}
	_pdata = WigListSmooth( _data, imager->xspace, _sws, axes.xmin, xincr );
	if( _pdata.size() == 0 ) {
		setCritical( "Nothing to plot" );
	}
	return( errorState() );
}
bool	MyApp::runBounds()
{
qDebug() << "runBounds " << _input.size() << " " << _input.at( 0 ).data.size();
qDebug() << "runBounds " << _features.size() << " " << _features.at( 0 ).data.size();
	QFuture<WigList> f = RunFuture<WigList>(
		QtConcurrent::run( Bounds, _input, _features, _dws, _sws ),
		isGui() );

	_data = f.result();

/*
	_data = RunBounds( _input, _features, _dws, _sws, isGui() );
	//_data = Bounds_wrapper( _input, _features, _dws, _sws, isGui() );
*/

	if( _data.size() == 0 ) {
		setWarning( "Features/Input gave nothing in results" );
	}
	return( errorState() );
}
void	MyApp::doPreCalc()
{
	if( !doMethod() ) {
		return;
	}
	if( !doPixels() ) {
		return;
	}
	// on empty _pdata will just set both min/max to ZERO
	//
	getYRange();
	_usedPreCalc = true;
	emit( preCalcFinished( true ) );
}
QImage*	MyApp::doPlot()
{
	QImage*	rv;

	if( _usedPreCalc ) {
		// get the Ymin/max stuff
		_ymin = D( param( "YMin" )->value() );
		_ymax = D( param( "YMax" )->value() );
	} else if( !doMethod() ) {
		rv = new QImage();
		return( rv );
	}
	if( !doPixels() ) {
		rv = new QImage();
		return( rv );
	}
	rv = doImage();
	return( rv );
}

	//return( MakeImage( _pdata ) );
/*
	if( !_usedPreCalc ) {
		doPreCalc();
		emit( preCalcFinished( false ) );
		_usedPreCalc = false;
	} else {
		qDebug() << "precalc";
	}
	return( doImage() );
*/

void	MyApp::getYRange()
{
	double	initValue = 0;

	if( _pdata.size() > 0 ) {
		initValue = _pdata.at( 0 ).val;
	}
	_ymin = _ymax = initValue;

	for( int i = 1; i < _pdata.size(); ++i ) {
		if( _pdata.at( i ).val > _ymax ) {
			_ymax = _pdata.at( i ).val;
		}
		if( _pdata.at( i ).val < _ymin ) {
			_ymin = _pdata.at( i ).val;
		}
	} 
	param( "YMin" )->setValue( V( _ymin ) );
	param( "YMax" )->setValue( V( _ymax ) );
}
QImage*	MyApp::doImage()
{
	QString	_imageTitle = S( param( "ImageTitle" )->value() );

	Scene	*scene = new Scene();

	if( _usedPreCalc ) {
		axes.yinit( _ymin, _ymax );
	} else {
		axes.yinit( _pdata );
	}
	imager->initScene( scene );
	imager->setGraphicFont( FONT_AXIS, "Courier", 10 );
	imager->setGraphicFont( FONT_TITLE, "Arial", 12 );
	imager->setTitle( scene, _imageTitle );
	if( !imager->plotData( scene, axes, _pdata, CLR( param( "LineColor" )->value() ).name() ) ) {
		return( 0 );
	}
	imager->colorRightSide( scene, "lightgrey" );
        QList<qreal>    xlegend;
        QList<qreal>    ylegend;

        int     nleg = 4;

        qreal legincr = ( axes.ymax - axes.ymin ) / nleg;
        for( qreal leg = axes.ymin; leg <= axes.ymax; leg += legincr ) {
                ylegend << leg;
        }
        if( ylegend.size() < nleg + 1 ) {
                ylegend << axes.ymax;
        }
        legincr = ( axes.xmax - axes.xmin ) / nleg;
        for( int leg = axes.xmin; leg <= axes.xmax; leg += legincr ) {
                xlegend << leg;
        }
        if( xlegend.size() < nleg + 1 ) {
                xlegend << axes.xmax;
        }
        imager->legendX( scene, axes, 30, 10, xlegend );
        imager->legendY( scene, axes, 30, 10, ylegend );

	QImage *rv = new QImage( _imageWidth, _imageHeight,
			 QImage::Format_ARGB32 );
			// QImage::Format_ARGB32_Premultiplied );
	QPainter* painter = new QPainter( rv );
	scene->render( painter );

	delete painter;
	delete scene;
	return( rv );
}
bool	MyApp::loadInput()
{
	WigLoader	wigLoader;

	if( !wigLoader.load( param( "InputFile" )->value(),
	 _input, isGui() ) ) {
		setCritical( "Unable to load input file" );
	}
	return( errorState() );
}
bool	MyApp::loadFeature()
{
	BedLoader	bedLoader;

	if( !bedLoader.load( param( "FeatureFile" )->value(),
	 _features, isGui() ) ) {
		setCritical( "Unable to load feature file" );
	}
	return( errorState() );
}
}	//	GH namespace


/*
QImage*	MyApp::doPlot()
{
	*
 	 *	GIVE UP FOR NOW THE SUBROUTINE WILL NOT WORK
	 *
	QImage* image = Plot( _data, _dws, _sws,
	 _imageTitle, _imageWidth, _imageHeight );
	return( image );
	return( Plot( _data, _dws, _sws,
	 _imageTitle, _imageWidth, _imageHeight ) );
 	 *

	int	_dws = param( "DWS" ).toInt();
	int	_sws = param( "SWS" ).toInt();
	int	_imageWidth = param( "ImageWidth" ).toInt();
	int	_imageHeight = param( "ImageHeight" ).toInt();
	QString	_imageTitle = param( "ImageTitle" ).toString();

	Axes	axes;
	Imager	*imager = new Imager( _imageWidth, _imageHeight, 60, 60, 60, 60 );
	Scene	*scene = new Scene();

	axes.xinit( ( _dws * -1 ), _dws );

	int	xincr;
	if( ( xincr = axes.xrange / imager->xspace ) <= 0 ) {
		xincr = 1;
	}
	_pdata = WigListSmooth( _data, imager->xspace, _sws, axes.xmin, xincr );

	{
		int i = 0;
		_ymin = _ymax = _pdata.at( i ).val;
		for( ++i; i < _pdata.size(); ++i ) {
			if( _pdata.at( i ).val > _ymax ) {
				_ymax = _pdata.at( i ).val;
			}
			if( _pdata.at( i ).val < _ymin ) {
				_ymin = _pdata.at( i ).val;
			}
		} 
	}
	setParam( "YMin", _ymin );
	setParam( "YMax", _ymax );

	axes.yinit( _pdata );
	imager->initScene( scene );
	imager->setGraphicFont( FONT_AXIS, "Courier", 10 );
	imager->setGraphicFont( FONT_TITLE, "Arial", 12 );
	imager->setTitle( scene, _imageTitle );
	if( !imager->plotData( scene, axes, _pdata, "black" ) ) {
		return( 0 );
	}
	imager->colorRightSide( scene, "lightgrey" );
	
        QList<qreal>    xlegend;
        QList<qreal>    ylegend;

        int     nleg = 4;

        qreal legincr = ( axes.ymax - axes.ymin ) / nleg;
        for( qreal leg = axes.ymin; leg <= axes.ymax; leg += legincr ) {
                ylegend << leg;
        }
        if( ylegend.size() < nleg + 1 ) {
                ylegend << axes.ymax;
        }
        legincr = ( axes.xmax - axes.xmin ) / nleg;
        for( int leg = axes.xmin; leg <= axes.xmax; leg += legincr ) {
                xlegend << leg;
        }
        if( xlegend.size() < nleg + 1 ) {
                xlegend << axes.xmax;
        }
        imager->legendX( scene, axes, 30, 10, xlegend );
        imager->legendY( scene, axes, 30, 10, ylegend );

	QImage *rv = new QImage( _imageWidth, _imageHeight,
			 QImage::Format_ARGB32 );
			// QImage::Format_ARGB32_Premultiplied );
	QPainter* painter = new QPainter( rv );
	scene->render( painter );

	delete painter;
	delete scene;
	return( rv );
}
*/
