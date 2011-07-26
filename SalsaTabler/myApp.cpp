#include "myApp.h"

namespace	GH
{
	MyApp::MyApp( int argc, char** argv )
	: CliApp( argc, argv )
{
	_inputColumnCount = 3;
	cout.open( STDOUT );
	cerr.open( STDERR );

	//
	//	Input and Output files
	//
	addParam( "BedInput", "", ParamModel::File, "Bed Input" );
	addParam( "WigInput", "", ParamModel::File, "Wig Input" );
	addParam( "Output", "", ParamModel::File, "OutputFile" );

	addParam( "WigLabel", "", ParamModel::Edit, "Column Label" );

	addParam( "UpZone", "true", ParamModel::Boolean, "Upstream Zone" );
	addParam( "UpZone_UpDist", "1000", ParamModel::Edit, "up" );
	addParam( "UpZone_DnDist", "1000", ParamModel::Edit, "dn" );
	addParam( "Zone", "false", ParamModel::Boolean, "Entire Zone" );
	addParam( "Zone_UpDist", "1000", ParamModel::Edit, "up" );
	addParam( "Zone_DnDist", "1000", ParamModel::Edit, "dn" );
	addParam( "DnZone", "false", ParamModel::Boolean, "Downstream Zone" );
	addParam( "DnZone_UpDist", "1000", ParamModel::Edit, "up" );
	addParam( "DnZone_DnDist", "1000", ParamModel::Edit, "dn" );
	addParam( "Exon", "false", ParamModel::Boolean,
		 "Over Exons (if specified in Bed File)" );
	addParam( "Exon_UpDist", "1000", ParamModel::Edit, "up" );
	addParam( "Exon_DnDist", "1000", ParamModel::Edit, "dn" );
	addParam( "Intron", "false", ParamModel::Boolean,
		 "Over Introns (if specified in Bed File)" );
	addParam( "Intron_UpDist", "1000", ParamModel::Edit, "up" );
	addParam( "Intron_DnDist", "1000", ParamModel::Edit, "dn" );

	addParam( "MeanOnCount", "false", ParamModel::Boolean,
	 "Mean calculated with the number of elements" );
	addParam( "MeanOnLength", "false", ParamModel::Boolean,
	 "Mean calculated over the length of each element" );
	addParam( "Median", "false", ParamModel::Boolean );
	addParam( "Variance", "false", ParamModel::Boolean );
	addParam( "Count", "false", ParamModel::Boolean );

	// need load actions
	addParam( "OpenExcel", "", ParamModel::Action, "Open in Excel" );
	addParam( "Doit", "", ParamModel::Action, "Run" );

//	tableModel = new TableModel( this );
	_tableHeader << "sid" << "beg" << "end";
//	tableModel->setExternalData( &tableData );
// tableModel->setExternalHeader( &tableHeader );

	_isGui = !checkArgs();
}
QStringList	MyApp::tableHeader() const
{
	return( _tableHeader );
}
QList<QList<QVariant> >	MyApp::tableData() const
{
	return( _tableData );
}
/*
TableModel*	MyApp::getModel()
{
	return( tableModel );
}
*/
bool	MyApp::openInExcel()
{
	QString fname = S( param( "Output" )->value() );
	bool	rv;

	if( fname.isEmpty() ) {
		fname = Tfp::TempPath( "XXXXXX.xls" );
		return( false );
	}
	Ofp	fp;
	fp.open( fname );
	rv = dumpToFile( fp );
	fp.close();

	if( rv ) {
		OpenWith( fname );
	}
	return( rv );
}
bool	MyApp::dumpToFile( QTextStream& fp )
{
	if( _tableData.size() == 0 ) {
		emit( emitError( Warning( "Nothing to see" ) ) );
		return( false );;
	}
	fp << _tableHeader.join( "," ) << endl;
	for( int i = 0; i < _tableData.size(); ++i ) {
		QStringList tsl;
		foreach( QVariant v, _tableData[ i ] ) {
			tsl << v.toString();
		}
		fp << tsl.join( "," ) << endl;
	}
	return( true );
}
void	MyApp::deleteColumn( const QList<int>& cols )
{
	QList<int>	rmCols;
	foreach( int c, cols ) {
		if( c >= _inputColumnCount ) {
			rmCols << c;
		}
	}
	if( rmCols.size() <= 0 )  {
	// removed a message from here, not sure how it could have happened
	// that targets (in window.cpp) has stuff and then rmCols is empty
	//
		return;
	}
	//tableModel->beginChange();
	for( int i = 0; i < _tableData.size(); ++i ) {
		foreach( int j, rmCols ) {
			_tableData[ i ].removeAt( j );
		}
	}
	foreach( int j, rmCols ) {
		_tableHeader.removeAt( j );
	}
	emit( newTableData() );
	//tableModel->endChange();
}
bool	MyApp::doit()
{
	if( _bedInput.size() == 0 && !loadBedInput() ) {
		return( errorState() );
	}
	if( _wigInput.size() == 0 && !loadWigInput() ) {
		return( errorState() );
	}

	data.clear();
	for( int i = 0; i < _bedInput.size(); ++i ) {
		Dol	dol;
		dol.sid = _bedInput[ i ].sid;
		int wi = SidSetIndex<WigSet>( _wigInput, dol.sid );
		if( wi != UINT ) {
			dol.wig = _wigInput[ wi ].data;
		} else {
			dol.wig = QList<Wig>();
		}
		dol.bed = _bedInput[ i ].data;
		foreach( QString p, paramNames() ) {
			dol.setParam( p, param( p )->value() );
		}
		data << dol;
	}
	//
	// make it be Dol's job to make sure that result maps 1-to-1 with bed
	//
	RunFuture<void>( QtConcurrent::map( data, &Dol::work ), isGui() );

	// here we can check for errors
	if( data.size() == 0 ) {
		setCritical( "Zero results, suggests there was no input?" );
	}
	//
	// this should be handled in Dol
	//
	if( data.at( 0 ).hdr.size() == 0 ) {
		setCritical(
		 "Results are corrupt, something went horribly wrong" );
	}
	for( int i = 0; i < data.size(); ++i ) {
		if( data.at( i ).hasError() ) {
			setError( data.at( i ).error() );
			break;
		}
	}
	if( hasError() ) {
		showError();
	} else {
		addToTable();
	}
	return( errorState() );
}
bool	MyApp::addToTable()
{
	int	trow = 0;

	//tableModel->beginChange();
	_tableHeader << data.at( 0 ).hdr;
	for( int i = 0; i < data.size(); ++i ) {
		for( int i2 = 0; i2 < data.at( i ).data.size(); ++i2 ) {
			for( int j = 0; j < data.at( i ).hdr.size(); ++j ) {
				_tableData[ trow ] << 
				//tableData[ trow ] <<
				 QVariant( data.at( i ).data.at( i2 ).at( j ) );
			}
			++trow;
		}
	}
	emit( newTableData() );
	//tableModel->endChange();
	return( errorState() );
}
bool	MyApp::loadBedInput()
{
	BedLoader	bedLoader;

	if( !bedLoader.load( param( "BedInput" )->value(), _bedInput, isGui() ) ) {
		setError( bedLoader.error() );
		showError();
	} else {
		updateTable();
	}
	return( errorState() );
}
bool	MyApp::updateTable()
{
	//tableModel->beginChange();
	_tableData.clear();
	_tableHeader = HEADER.split( "," );

	foreach( BedSet bs, _bedInput ) {
		foreach( Bed b, bs.data ) {
			QList<QVariant> t;
			t << QVariant( b.sid );
			t << QVariant( b.beg );
			t << QVariant( b.end );
			t << QVariant( b.aux );
			t << QVariant( b.label );

			_tableData << t;
		}
	}
	emit( newTableData() );
	//tableModel->endChange();

	return( errorState() );
}
bool	MyApp::loadWigInput()
{
	WigLoader	wigLoader;

	if( !wigLoader.load( param( "WigInput" )->value(),  _wigInput, isGui() ) ) {
		setError( wigLoader.error() );
		showError();
	}
	return( errorState() );
}
}	//	GH namespace
