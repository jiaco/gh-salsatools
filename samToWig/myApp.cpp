#include "myApp.h"

namespace	GH
{
	MyApp::MyApp( int argc, char** argv )
	: CliApp( argc, argv )
{
	cout.open( STDOUT );
	cerr.open( STDERR );

	addParam( "i", ParamModel::File,
	 "displayName=Input" );
	addParam( "v", ParamModel::Edit,
	 "displayName=Vstep Size;"
	 "defaultValue=200;" );

	addParam( "g", ParamModel::Edit,
	 "displayName=Genome key;"
	 "defaultValue=hsa;" );

	addParam( "d", ParamModel::Boolean,
	 "displayName=DebugMode;"
	 "defaultValue=false;" 
	);

	setRequiredParamNames( QStringList() << "i" );
	_isGui = !checkArgs();
}
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
		cerr << "Problem with " << inputFile << endl;
		return;
	}

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
	ifp.close();
	return;
}
}	//	GH namespace
