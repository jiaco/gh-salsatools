#include "myApp.h"

namespace	GH
{
QString	SaneSid( const QString& sid )
{
	QString	rv = sid;
	QString	g = APP_S( "g" );
	if( rv.contains( '.' ) ) {
		int x = rv.indexOf( '.' );
		rv = rv.mid( 0, x );
	}
	if( !g.isEmpty() ) {
		rv = QString( "%1_%2" ).arg( APP_S( "g" ) ).arg( rv );
	}
	return( rv );
}


	MyApp::MyApp( int argc, char** argv )
	: CliApp( argc, argv )
{
	cout.open( STDOUT );
	cerr.open( STDERR );

	addParam( "i", ParamModel::File,
	 "displayName=Input;"
	 );
	addParam( "v", ParamModel::Edit,
	 "displayName=Vstep Size;"
	 "defaultValue=200;"
	 );
	addParam( "g", ParamModel::Edit,
	 "displayName=Genome key;"
	 "defaultValue=hsa;"
	 );
	addParam( "d", ParamModel::Boolean,
	 "displayName=DebugMode;"
	 "defaultValue=false;" 
	);

	_runChoices
	 << SPLITSORT
	 << WIGOUT
	 << EXPORT2BAM
	 << EXPORT2BED
	 << BAM2BED
	;

	addParam( "x", ParamModel::Choice,
	 "displayName=Method;"
	 "defaultValue=" % EXPORT2BED % ";"
	);


	setRequiredParamNames( QStringList() << "i" );
	_isGui = !checkArgs();
}
bool	MyApp::readyInput()
{
	_inputFormat = Undef;
	_inputFile = APP_S( "i" );
	if( _inputFile.endsWith( ".bam" ) ) {
		_inputFormat = Bam;
	} else if( _inputFile.endsWith( "_export.txt" ) ) {
		_inputFormat = Export;
	} else {
		setCritical( "Unsupported input format."
		 "Make sure file ends with .bam or _export.txt" );
		return( false );
	}
	if( !ifp.open( _inputFile ) ) {
		setError( ifp.error() );
		return( false );
	}
	return( true );
}
bool	MyApp::doit()
{
	bool	rv = false;

	QString	methodToUse = APP_S( "x" );
	if( !readyInput() ) {
		return( false );
	}
	if( methodToUse == SPLITSORT ) {
		rv = splitSort();
	} else if( methodToUse == WIGOUT ) {
		rv = wigOut();
	} else if( methodToUse == EXPORT2BAM ) {
		rv = exportToBam();
	} else if( methodToUse == EXPORT2BED ) {
		rv = exportToBed();
	} else if( methodToUse == BAM2BED ) {
		rv = bamToBed();
	} else {
		setCritical( "Somehow selected bad method type" );
		rv = false;
	}
	ifp.close();
	return( rv );
}
bool	MyApp::wigOut()
{
	return( true );
}
bool	MyApp::splitSort()
{
	return( true );
}
	// GH BED CLASS:
	// sid beg end label val aux
bool	MyApp::bamToBed()
{
	if( _inputFormat != Bam ) {
		return( false );
	}
	return( true );
}
bool	MyApp::exportToBed()
{
	if( _inputFormat != Export ) {
		return( false );
	}
	Token	tok;
	int	nLines = 0;
	int	nOut = 0;
	int	nSkip = 0;

	QMap<QString,QList<Bed> >	hold;

	tok.setEmptyParts();
	while( tok.next( ifp ) ) {
		++nLines;
/*
		if( tok.at( tok.size() - 1 ) == "N" ) {
			continue;
		}
*/
		Bed bed;

/*
		if( !tok.at( EXPORT_XXX ).isEmpty() ) {
			cerr << "XXX: " << tok.line() << endl;
		}
*/

		bed.sid = tok.at( EXPORT_SID );
		if( !bed.sid.endsWith( ".fa" ) ) {
			++nSkip;
			continue;
		}
		bed.beg = tok.at( EXPORT_BEG ).toInt();
		bed.end = bed.beg + tok.at( EXPORT_SEQ ).size();
		bed.label = formIdFromExport( tok );
		bed.val = 0.0;
		bed.aux = ( tok.at( EXPORT_AUX ) == "F" ) ? 0 : 1;


		if( !hold.contains( bed.sid ) ) {
			QList<Bed> t;
			t << bed;
			hold.insert( bed.sid, t );
		} else {
			hold[ bed.sid ] << bed;
		}

	//	bed.showBed( cout );

		++nOut;
	}
	cerr << "Done reading: "
	 << nOut << " out of " << nLines << " for output"
		<< " skipped: " << nSkip << endl;

	foreach( QString sid, hold.keys() ) {
		cerr << sid << "\t" << hold[ sid ].size() << endl;
		qSort( hold[ sid ].begin(), hold[ sid ].end(), BedSort );
		for( int i = 0; i < hold[ sid ].size(); ++i ) {
			hold[ sid ].at( i ).showBed( cout );
		}
	}
	return( true );
}
bool	MyApp::exportToBam()
{
	// GAVE UP AS NO IDEA HOW THEY GET LOTS OF THESE BAM FIELDS
	// AND WE DONT NEED BAM ANYWAY
	if( _inputFormat != Export ) {
		return( false );
	}
	Token	tok;
	tok.setEmptyParts();
	int	nLines = 0;
	QString	sid;
	int	aux;

	while( tok.next( ifp ) ) {
		++nLines;
		if( nLines > 10 ) {
			break;
		}
		if( tok.at( tok.size() - 1 ) == "N" ) {
			continue;
		}
		sid = formIdFromExport( tok );
		aux = ( tok.at( EXPORT_AUX ) == "F" ) ? 0 : 1;
	
		cout << sid << "\t" << tok.line() << endl;
	}

	return( true );
}
QString	MyApp::formIdFromExport( const Token& tok ) const
{
	return( QString( "%1_%2:%3:%4:%5:%6" )
	 .arg( tok.at( 0 ) )
	 .arg( tok.at( 1 ) )
	 .arg( tok.at( 2 ) )
	 .arg( tok.at( 3 ) )
	 .arg( tok.at( 4 ) )
	 .arg( tok.at( 5 ) ) );
}
/*
void	MyApp::dumpD( QTextStream& fp, const QString& sid,
	 QVector<int>& tab )
{
	int	lastPos = UINT;
	int	pos;

	Q_UNUSED( sid );

	for( int i = 0; i < tab.size(); ++i ) {
		pos = tab.at( i );
		if( pos != lastPos ) {
			if( lastPos != UINT ) {
				fp << pos - lastPos << endl;
			}
			lastPos = pos;
		}
	}
}
void	MyApp::dump( QTextStream& fp, const QString& sid,
	 QVector<int>& tab )
{
	int	pos, epos, cnt;
	int	step = APP_I( "v" );

	pos = cnt = 0;

	if( tab.size() == 0 ) {
		return;
	}
	pos = tab.at( 0 );
	epos = step + pos;

	fp << "variableStep chrom=" << SaneSid( sid )
	 << " span=" << step
	 << endl;
	for( int i = 0; i < tab.size(); ) {
		if( tab.at( i ) > epos ) {
			if( cnt > 0 ) {
				fp << pos << "\t" << cnt << endl;
			}
			pos = tab.at( i );
			epos = step + pos;
			cnt = 0;
		} else {
			++cnt;
			++i;
		}
	}
	if( cnt > 0 ) {
		fp << pos << "\t" << cnt << endl;
	}
}
void	MyApp::doit()
{
	QString	inputFile = APP_S( "i" );
	Ifp	ifp;
	Token	tok;
	QString	sid, lastSid;
	QVector<int>	btab;

	bool	debugMode = APP_B( "d" );

	if( !ifp.open( inputFile ) ) {
		setError( ifp.error() );
		cerr << "Error: " << error()->message() << endl;
		cerr << "Problem with " << inputFile << endl;
		return;
	}


	QString line;
	for( int i = 0; i < 10 && nextLine( ifp, line ); ++i ) {
		cout << "DEBUG " << line << endl;
	}

*
	if( !debugMode ) {
		cout << "track type=wiggle_0\n";
	}
	lastSid = USTR;
	while( tok.next( ifp ) ) {
		if( tok.size() < MIN_TOK ) {
			continue;
		}
		sid = tok.at( SID_AT );
		if( sid != lastSid ) {
			lastSid = sid;
			if( btab.size() > 0 ) {
				if( debugMode ) {
					dumpD( cout, lastSid, btab );
				} else {
					dump( cout, lastSid, btab );
				}
			}
			btab.clear();
		}
		btab << tok.at( BEG_AT ).toInt();
	}
	if( btab.size() > 0 ) {
		if( debugMode ) {
			dumpD( cout, lastSid, btab );
		} else {
			dump( cout, lastSid, btab );
		}
	}
*
	ifp.close();
	return;
}
*/
}	//	GH namespace
