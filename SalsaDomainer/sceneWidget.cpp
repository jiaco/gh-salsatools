#include "sceneWidget.h"

namespace	GH
{
void	PVatRes( const WigList& wig,
	 const int& beg, const qreal& res,
	 qreal& pos, qreal& val, const qreal& max )
{
	// returns a pos-val for a range of data between beg -> res(end)

	int lo, hi, end;
	qreal	tot = 0;

	lo = wig.at( beg ).beg;
	hi = lo;
	for( end = beg; end < beg + res && end < wig.size(); ++end ) {
		if( wig.at( end ).val > max ) {
			tot += max;
		} else {
			tot += wig.at( end ).val;
		}
		hi = wig.at( end ).beg;
	}
	pos = lo + ( hi - lo ) / 2;
	val = tot / res;
}
	SceneWidget::SceneWidget( const QString& title, MyApp* app,
	 QWidget* parent )
	: QWidget( parent, Qt::Window )
{
	this->app = app;
	_sid = title;	// NEED TO FIX/NAME THIS BETTER
	p_maxValue = APP_D( "maxvalue" );

	clrInput = QColor( 0, 0, 255, 255 );
	clrSites = QColor( 0, 0, 0, 255 );
	clrDomains = QColor( 0, 255, 0, 50 );
	clrPeaks = QColor( 255, 0, 0, 100 );
	clrSelected = QColor( 255, 0, 255, 150 );

	setWindowTitle( title );
	scene = new Scene( this );
	
	view = new View( scene, this );
	view->setScaleOp( SCALE_X_ONLY );
	view->setMoveOp( MOVE_X_ONLY );
	view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	_actSaveDomains = new QAction( tr( "Save Domains" ), this );
	connect( _actSaveDomains, SIGNAL( triggered() ),
		 this, SLOT( callSaveDomains() ) );
	connect( this, SIGNAL( saveDomains(QString,QString) ),
		app, SLOT( saveDomains(QString,QString) ) );

	_actDeleteDomains = new QAction( tr( "Delete row(s)" ), this );
	connect( _actDeleteDomains, SIGNAL( triggered() ),
		this, SLOT( callDeleteDomains() ) );
	_actMergeDomains = new QAction( tr( "Merge rows" ), this );
	connect( _actMergeDomains, SIGNAL( triggered() ),
		this, SLOT( callMergeDomains() ) );
	//
	//	this connection is used by merge & delete
	//
	connect( this, SIGNAL( domainsAltered(QString,BedList) ),
		app, SLOT( setDomains(QString,BedList) ) );

	_actOpenDomainInBrowser = new QAction( tr( "Open in Browser" ), this );
	connect( _actOpenDomainInBrowser, SIGNAL( triggered() ),
		 this, SLOT( callOpenDomainInBrowser() ) );
	_actOpenPeakInBrowser = new QAction( tr( "Open in Browser" ), this );
	connect( _actOpenPeakInBrowser, SIGNAL( triggered() ),
		 this, SLOT( callOpenPeakInBrowser() ) );


	_actSavePeaks = new QAction( tr( "Save Peaks" ), this );
	connect( _actSavePeaks, SIGNAL( triggered() ),
		 this, SLOT( callSavePeaks() ) );
	connect( this, SIGNAL( savePeaks(QString,QString) ),
		app, SLOT( savePeaks(QString,QString) ) );

	_actDeletePeaks = new QAction( tr( "Delete row(s)" ), this );
	connect( _actDeletePeaks, SIGNAL( triggered() ),
		this, SLOT( callDeletePeaks() ) );
	_actMergePeaks = new QAction( tr( "Merge rows" ), this );
	connect( _actMergePeaks, SIGNAL( triggered() ),
		this, SLOT( callMergePeaks() ) );
	//
	//	this connection is used by merge & delete
	//
	connect( this, SIGNAL( peaksAltered(QString,BedList) ),
		app, SLOT( setPeaks(QString,BedList) ) );

	
	_splitter = new QSplitter( this );
	_tabWidget = new QTabWidget( this );
	tableDomains = new TableWidget( this );
	tableDomains->setSelectionMode(QAbstractItemView::ContiguousSelection);
	tableDomains->setEditTriggers( QAbstractItemView::NoEditTriggers );
	tableDomains->setContextMenuPolicy( Qt::ActionsContextMenu );
	tableDomains->addAction( _actSaveDomains );
	tableDomains->addAction( _actDeleteDomains );
	tableDomains->addAction( _actMergeDomains );

	tableDomains->addAction( _actOpenDomainInBrowser );

	tablePeaks = new TableWidget( this );
	tablePeaks->setSelectionMode(QAbstractItemView::ContiguousSelection);
	tablePeaks->setContextMenuPolicy( Qt::ActionsContextMenu );
	tablePeaks->setContextMenuPolicy( Qt::ActionsContextMenu );
	tablePeaks->addAction( _actSavePeaks );
	tablePeaks->addAction( _actDeletePeaks );
	tablePeaks->addAction( _actMergePeaks );
	tablePeaks->addAction( _actOpenPeakInBrowser );

	tableSites = new TableWidget( this );
	tableSites->setSelectionMode(QAbstractItemView::ContiguousSelection);
	tableSites->setContextMenuPolicy( Qt::ActionsContextMenu );
	tableSites->setContextMenuPolicy( Qt::ActionsContextMenu );

	_controlWidget = new QWidget( this );
	EditResolution =
	 new QLineEdit( APP_S( "graphics/resolution" ), this );
	 //new QLineEdit( S( app->param( "Resolution" )->value() ), this );
	QVBoxLayout	*controlLayout = new QVBoxLayout;
	controlLayout->addWidget( new QLabel( "Resolution:" ) );
	controlLayout->addWidget( EditResolution );
	_controlWidget->setLayout( controlLayout );

	connect( EditResolution, SIGNAL( editingFinished() ),
		this, SLOT( redraw() ) );

	//p_Resolution = new EditParam( app, "Resolution", this );

	_tabWidget->addTab( tableDomains, "Domains" );
	_tabWidget->addTab( tablePeaks, "Peaks" );
	_tabWidget->addTab( tableSites, "Sites" );
	_tabWidget->addTab( _controlWidget, "Controls" );

	_splitter->addWidget( view );
	_splitter->addWidget( _tabWidget );

	connect( tableDomains, SIGNAL( cellClicked(int,int) ),
	 this, SLOT( tableDomainSelect(int,int) ) );
	connect( tablePeaks, SIGNAL( cellClicked(int,int) ),
	 this, SLOT( tablePeakSelect(int,int) ) );
	connect( tableSites, SIGNAL( cellClicked(int,int) ),
	 this, SLOT( tableSiteSelect(int,int) ) );

	connect( view, SIGNAL( itemClicked(QGraphicsItem*) ),
	 this, SLOT( viewItemClicked(QGraphicsItem*) ) );

	QGridLayout	*grid = new QGridLayout;
	grid->addWidget( _splitter );
	setLayout( grid );
	
	// now can query the app for graphic parameters
	// and can then process the input
	//
	updateGraphics();

	// need to ensure that this is only called once here in 
	// the constructor
	//
	setupInput();
	//redrawInput();
	redraw();
}
QList<int>	SceneWidget::getSelectedRows( QTableWidget* table )
{
	QModelIndexList idxList = table->selectionModel()->selectedIndexes();
	QList<int>	rv;

	for( int i = 0; i < idxList.size(); ++i ) {
		if( !rv.contains( idxList[ i ].row() ) ) {
			rv << idxList[ i ].row();
		}
	}
	return( rv );
}
void	SceneWidget::callOpenDomainInBrowser()
{
	openUcscTableSelection( tableDomains, _sid, app->domains( _sid ) );
}
void	SceneWidget::callOpenPeakInBrowser()
{
	openUcscTableSelection( tablePeaks, _sid, app->peaks( _sid ) );
}
void	SceneWidget::openUcscTableSelection( TableWidget* table,
		const QString& sid, const BedList& data )
{
	int	b, e;
	BedList	tlist;
	QList<int>	rowsToAct = getSelectedRows( table );

	if( rowsToAct.size() < 1 ) {
		return;
	}
	qSort( rowsToAct.begin(), rowsToAct.end() );
	b = rowsToAct.at( 0 );
	e = rowsToAct.at( rowsToAct.size() - 1 );

	openUcscUrl( sid, data.at( b ).beg,
		data.at( e ).end );
}
void	SceneWidget::openUcscUrl( const QString& sid,
		 const int& beg, const int& end )
{
	QString	ucscBase = "http://genome.ucsc.edu/cgi-bin/hgTracks?";
	QString	org = "Human";
	QString	db = "hg18";

	QString url = QString( "%1org=%2&db=%3&position=%4:%5-%6" )
	.arg( ucscBase )
	.arg( org ).arg( db )
	.arg( sid ).arg( beg ).arg( end );

	QDesktopServices::openUrl( QUrl( url ) );
}
void	SceneWidget::callMergeDomains()
{
	int	b, e;
	QList<int>	rowsToAct = getSelectedRows( tableDomains );

	if( rowsToAct.size() <= 1 ) {
		return;
	}
	qSort( rowsToAct.begin(), rowsToAct.end() );
	b = rowsToAct.at( 0 );
	e = rowsToAct.at( rowsToAct.size() - 1 );

	BedList	tlist;
	const BedList domains = app->domains( _sid );
	for( int i = 0; i < domains.size(); ++i ) {
		if( !rowsToAct.contains( i ) ) {
			tlist << domains[ i ];
		}
	}
	Bed t = domains[ b ];
	t.end = domains[ e ].end;
	t.val = UDBL;
	
	tlist << t;
	qSort( tlist.begin(), tlist.end(), BedSort );
	
	emit( domainsAltered( _sid, tlist ) );
}
void	SceneWidget::callMergePeaks()
{
	int	b, e;
	QList<int>	rowsToAct = getSelectedRows( tablePeaks );

	if( rowsToAct.size() <= 1 ) {
		return;
	}
	qSort( rowsToAct.begin(), rowsToAct.end() );
	b = rowsToAct.at( 0 );
	e = rowsToAct.at( rowsToAct.size() - 1 );

	BedList	tlist;
	const BedList peaks = app->peaks( _sid );
	for( int i = 0; i < peaks.size(); ++i ) {
		if( !rowsToAct.contains( i ) ) {
			tlist << peaks[ i ];
		}
	}
	Bed t = peaks[ b ];
	t.end = peaks[ e ].end;
	t.val = UDBL;
	
	tlist << t;
	qSort( tlist.begin(), tlist.end(), BedSort );
	
	emit( peaksAltered( _sid, tlist ) );
}
void	SceneWidget::callDeleteDomains()
{
	QList<int>	rowsToAct = getSelectedRows( tableDomains );
	BedList	tlist;
	const BedList	domains = app->domains( _sid );
	for( int i = 0; i < domains.size(); ++i ) {
		if( !rowsToAct.contains( i ) ) {
			tlist << domains[ i ];
		}
	}
	emit( domainsAltered( _sid, tlist ) );
}
void	SceneWidget::callDeletePeaks()
{
	QList<int>	rowsToAct = getSelectedRows( tablePeaks );
	BedList	tlist;
	const BedList	peaks = app->peaks( _sid );
	for( int i = 0; i < peaks.size(); ++i ) {
		if( !rowsToAct.contains( i ) ) {
			tlist << peaks[ i ];
		}
	}
	emit( peaksAltered( _sid, tlist ) );
}
void	SceneWidget::callSaveDomains()
{
	QString path = QFileDialog::getSaveFileName( this );
	emit( saveDomains( _sid, path ) );
}
void	SceneWidget::callSavePeaks()
{
	QString path = QFileDialog::getSaveFileName( this );
	emit( savePeaks( _sid, path ) );
}
void	SceneWidget::highlightDomain( const int& index )
{
	for( int i = 0; i < domlist.size(); ++i ) {
		domlist[ i ]->setPen( QPen( clrDomains ) );
	}
	domlist[ index ]->setPen( QPen( clrSelected ) );
}
void	SceneWidget::highlightPeak( const int& index )
{
	for( int i = 0; i < peaklist.size(); ++i ) {
		peaklist[ i ]->setPen( QPen( clrPeaks ) );
	}
	peaklist[ index ]->setPen( QPen( clrSelected ) );
}
void	SceneWidget::tableSiteSelect( const int& row, const int& col )
{
	Q_UNUSED( col );

	if( row < sitelist.size() ) {
		//highlightDomain( row );
		view->centerOn( sitelist.at( row ) );
	}
}
void	SceneWidget::tableDomainSelect( const int& row, const int& col )
{
	Q_UNUSED( col );

	if( row < domlist.size() ) {
		highlightDomain( row );
		view->centerOn( domlist.at( row ) );
	}
}
void	SceneWidget::tablePeakSelect( const int& row, const int& col )
{
	Q_UNUSED( col );

	if( row < peaklist.size() ) {
		highlightPeak( row );
		view->centerOn( peaklist.at( row ) );
	}
}
void	SceneWidget::viewItemClicked( QGraphicsItem* item )
{
	int	type, index;

	QString	s = item->data( GITEM_TYPE ).toString();
	if( s.isEmpty() ) {
		return;
	}
	type = s.toInt();
	index = item->data( GITEM_INDEX ).toInt();
	switch( type ) {
		case	GITEM_DOMAIN:
			tableDomains->setCurrentCell( index, 0 );
			highlightDomain( index );
			break;
		case	GITEM_PEAK:
			tablePeaks->setCurrentCell( index, 0 );
			highlightPeak( index );
			break;
		case	GITEM_SITE:
// it works but the line is 1 pixel wide and hard to hit with mouse
			tableSites->setCurrentCell( index, 0 );
			break;
		default:
			break;
	}
}
/*
void	SceneWidget::updateGraphics( const qreal& res,
	 const qreal& width, const qreal& height, const qreal& margin )
*/
void	SceneWidget::updateGraphics()
{
	_res = APP_D( "graphics/resolution" );
	_margin = APP_D( "graphics/margin" );
	_width = APP_D( "graphics/width" );
	_height = APP_D( "graphics/height" );

	//_res = res;
	g_x0 = g_y0 = _margin;// = margin;
	g_xmax = _width - ( 2 * _margin );
	g_ymax = _height - ( 2 * _margin );

	//view->setSceneRect( g_x0, g_y0, g_x0 + g_xmax, g_y0 + g_ymax );
	view->setSceneRect( g_x0, g_y0, g_xmax, g_ymax );
	resize( _width * 2, _height );
}
void	SceneWidget::setupInput()
{
	const WigList _input = app->input( _sid );
	if( _input.size() == 0 ) {
		return;
	}
	int	i = 0;
	d_xmax = _input.at( i ).beg;
	d_ymax = _input.at( i ).val;
	for( ; i < _input.size(); ++i ) {
		if( _input.at( i ).beg > d_xmax ) {
			d_xmax = _input.at( i ).beg;
		}
		if( _input.at( i ).val > d_ymax ) {
			d_ymax = _input.at( i ).val;
		}
	}
	if( d_ymax > p_maxValue ) {
		d_ymax = p_maxValue;
	}

	QString	oldTitle = windowTitle();
	setWindowTitle( QString( "%1 - %2 [ %3 ]" ).arg( oldTitle ).arg( d_ymax ).arg( p_maxValue ) );

}
void	SceneWidget::redrawInput()
{
	const WigList _input = app->input( _sid );
	QGraphicsTextItem 	*gti;

	if( _input.size() == 0 ) {
		return;
	}
	pen = QPen( clrInput );
	qreal	x0, y0, x1, y1;
	qreal	pos, val;	//, redline, redval;

	scene->clear();
	int i = 0;

	_res = EditResolution->text().toDouble();

	PVatRes( _input, i, _res, pos, val, p_maxValue );

	gti = scene->addText( QString( "%1" ).arg( pos, 0, 'f' ) );
	gti->setPos( g_x0 + ( g_xmax * ( pos / d_xmax ) ), _margin );
	gti->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );

	x0 = g_x0 + ( g_xmax * ( pos / d_xmax ) );
	y0 = g_y0 + ( g_ymax * ( val / p_maxValue ) );
	for( i += _res; i < _input.size(); i += _res ) {
		PVatRes( _input, i, _res, pos, val, p_maxValue );
		x1 = g_x0 + ( g_xmax * ( pos / d_xmax ) );
		y1 = g_y0 + ( g_ymax * ( val / p_maxValue ) );
		//
		// here can log things that go out of range...
		//
		scene->addLine( x0, y0, x1, y1, pen );
		x0 = x1;
		y0 = y1;
	}
	gti = scene->addText( QString( "%1" ).arg( pos, 0, 'f' ) );
	gti->setPos( g_x0 + ( g_xmax * ( pos / d_xmax ) ), _margin );
	gti->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );

	pos /= 2;
	gti = scene->addText( QString( "%1" ).arg( pos, 0, 'f' ) );
	gti->setPos( g_x0 + ( g_xmax * ( pos / d_xmax ) ), _margin );
	gti->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );

	double	hline = APP_D( "domains/minzonescore" );
	hline = g_y0 + ( g_ymax * ( hline / p_maxValue ) );
	scene->addLine( g_x0, hline, g_xmax, hline, QPen( QColor( "red" ) ) );
/*
	emit( message( QString( "Capped %1" ).arg( nCapped ) ) );
 * could not get nice text

	QFont	legFont = QFont( "Courier", 2 );
	QGraphicsTextItem	*sti;
	for( int xleg = 1; xleg < d_xmax; xleg += 1000000 ) {
		sti = scene->addText( QString( "%1" ).arg( xleg ), legFont );
		x1 = g_x0 + ( g_xmax * ( xleg / d_xmax ) );
		y1 = g_y0 * 3;
		sti->setPos( x1, y1 );
	}
*/
}
void	SceneWidget::redoBedList( const int& item_type, const BedList& data,
	 const QColor& color, QList<QGraphicsRectItem*>& itab,
	 TableWidget* table, const QStringList& columns )
{
	qreal	x, w, end;

	itab.clear();
	table->clear();
	table->setupRowsAndColumns( data.size(), columns );
	pen = QPen( color );
	brush = QBrush( color );
	for( int i = 0; i < data.size(); ++i ) {
		x = g_x0 + ( g_xmax * ( data.at( i ).beg / d_xmax ) );
		end = g_x0 + ( g_xmax * ( data.at( i ).end / d_xmax ) );
		w = end - x;
		QGraphicsRectItem *it =
		 scene->addRect( x, g_y0, w, g_ymax, pen, brush );
		it->setData( GITEM_TYPE, item_type );
		it->setData( GITEM_INDEX, i );
		itab << it;

		table->setItem( i, 0,
		 new QTableWidgetItem( tr("%1").arg( data.at( i ).beg )) );
		table->setItem( i, 1,
		 new QTableWidgetItem( tr("%1").arg( data.at( i ).end )) );
		table->setItem( i, 2,
		 new QTableWidgetItem( tr("%1").arg( data.at( i ).val )) );
	}
}
void	SceneWidget::redrawSites()
{
	QStringList	columns;
	columns << "beg" << "end" << "val";

	redoBedList( GITEM_SITE, app->sites( _sid ), clrSites, sitelist, tableSites, columns );
}
void	SceneWidget::redrawDomains()
{
	QStringList	columns;
	columns << "beg" << "end" << "val";

	redoBedList( GITEM_DOMAIN, app->domains( _sid ), clrDomains, domlist, tableDomains, columns );
}
void	SceneWidget::redrawPeaks()
{
	QStringList	columns;
	columns << "beg" << "end" << "val";

	redoBedList( GITEM_PEAK, app->peaks( _sid ), clrPeaks, peaklist, tablePeaks, columns );
}
void	SceneWidget::redraw()
{
	redrawInput();
	redrawSites();
	redrawDomains();
	redrawPeaks();
}
/*
 * duh, signals dont need methods
void	SceneWidget::message( const QString& text, const int& type )
{
}
*/
}	//	GH namespace
