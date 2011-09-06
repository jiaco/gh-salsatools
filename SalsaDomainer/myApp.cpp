#include "myApp.h"

namespace	GH
{
	MyApp::MyApp( int argc, char** argv )
	: CliApp( argc, argv )
{
	_isGui = true;
	_currentInputIndex = 0;
	cout.open( "stdout" );

	addParam( "currentsid", ParamModel::Choice,
	 "defaultValue='';"
	 "displayName=Sid;"
	 );
	addParam( "maxvalue", ParamModel::Edit,
	 "defaultValue=12;"
	 "displayName=Max;"
	 );

	addParam( "input/file", ParamModel::File,
	 "displayName=Input File;"
	 "dialogType=OpenMulti;"
	 "caption=Select input wig file(s);"
	 "filter=" % WIG_SUFFIX % ";"
	 );
/*
	addParam( "input/resolution", ParamModel::Edit,
	 "displayName=Resolution;"
	 "defaultValue=50;"
  	 );
*/
	addParam( "input/load", ParamModel::Action,
	 "displayName=Load;"
	 );

	addParam( "domains/file", ParamModel::File,
	 "displayName=Domains File;"
	 "dialogType=OpenMulti;"
	 "caption=Select bed domain file(s);"
	 "filter=" % BED_SUFFIX % ";"
	 );
	addParam( "domains_/load", ParamModel::Action,
	 "displayName=Load Domains;"
	 );
	addParam( "domains/minseedvalue", ParamModel::Edit,
	 "displayName=Min Seed Value;"
	 "defaultValue=2;"
	 );
	addParam( "domains/minseednumprobes", ParamModel::Edit,
	 "displayName=Min Seed Num Probes;"
	 "defaultValue=80;"
	 );
	addParam( "domains/minseedscore", ParamModel::Edit,
	 "displayName=Min Seed Score;"
	 "defaultValue=3.2;"
	 );
	addParam( "domains/mincoverage", ParamModel::Edit,
	 "displayName=Min Coverage;"
	 "defaultValue=0.001;"
	 );
	addParam( "domains/mergedistance", ParamModel::Edit,
	 "displayName=Merge Distance;"
	 "defaultValue=200000;"
	 );
	addParam( "domains/minzonesize", ParamModel::Edit,
	 "displayName=Min Zone Size;"
	 "defaultValue=200000;"
	 );
	addParam( "domains/minzonescore", ParamModel::Edit,
	 "displayName=Min Zone Score;"
	 "defaultValue=2.2;"
	 );
	addParam( "domains/extendwidth", ParamModel::Edit,
	 "displayName=Extend Width;"
	 "defaultValue=40;"
	 );
	addParam( "domains/extendpctover", ParamModel::Edit,
	 "displayName=Extend %Over;"
	 "defaultValue=0.7;"
	 );
	addParam( "domains/runall", ParamModel::Boolean,
	 "displayName=Run All;"
	 "tooltip=Run on all loaded chromosomes;"
	 "defaultValue=false;"
	 );
	addParam( "domains/run", ParamModel::Action,
	 "displayName=Find Domains;"
	 );

	addParam( "peaks/file", ParamModel::File,
	 "displayName=Peaks File;"
	 "dialogType=OpenMulti;"
	 "caption=Select bed peaks file(s);"
	 "filter=" % BED_SUFFIX % ";"
	 );
	addParam( "peaks_/load", ParamModel::Action,
	 "displayName=Load Peaks;"
	 );
	addParam( "peaks/minnumprobes", ParamModel::Edit,
	 "displayName=Min Num Probes;"
	 "defaultValue=40;"
	 );
	addParam( "peaks/minpctavg", ParamModel::Edit,
	 "displayName=Min %Average;"
	 "defaultValue=1.0;"
	 );
	addParam( "peaks/minpctover", ParamModel::Edit,
	 "displayName=Min %Over;"
	 "defaultValue=0.7;"
	 );
	addParam( "peaks/mergedistance", ParamModel::Edit,
	 "displayName=Merge Distance;"
	 "defaultValue=10000;"
	 );
	addParam( "peaks/runall", ParamModel::Boolean,
	 "displayName=Run All;"
	 "tooltip=Run on all loaded chromosomes;"
	 "defaultValue=false;"
	 );
	addParam( "peaks/run", ParamModel::Action,
	 "displayName=Find Peaks;"
	 );
	addParam( "graphics/height", ParamModel::Edit,
	 "displayName=Height;"
	 "defaultValue=600"
	 );
	addParam( "graphics/width", ParamModel::Edit,
	 "displayName=Width;"
	 "defaultValue=600"
	 );
	addParam( "graphics/margin", ParamModel::Edit,
	 "displayName=Margin;"
	 "defaultValue=10"
	 );
	addParam( "graphics/resolution", ParamModel::Edit,
	 "displayName=Resolution;"
	 "defaultValue=50"
	 );

	BedLoader bedLoader;
	bedLoader.load( ":/_static/human_asisi.bed", _siteData, false );
}
void	MyApp::doit()
{
	if( !isGui() ) {
/*
	this is totally silly as it only handles 1 parameter

		QFileInfo fi( args[ "i" ] );
		if( fi.exists() && fi.isFile() ) {
			setParam( "FileInput", args[ "i" ] );
		} else if( fi.isDir() ) {
			setParam( "FileInput", Scan( args[ "i" ],
			 QStringList() << "wig" ) );
		} else {
			cout << "Bad Input Param" << endl;
			return;
		}
*/
	}
	loadInput();
	doDomainFind();
}
const WigList	MyApp::input( const QString& sid ) const
{
	int i = UINT;

	if( ( i = SidSetIndex<WigSet>( _input, sid ) ) == UINT ) {
		return( WigList() );
	}
	return( _input.at( i ).data );
}
const	BedList	MyApp::sites( const QString& sid ) const
{
	int	i = UINT;

	if( ( i = SidSetIndex<BedSet>( _siteData, sid ) ) == UINT ) {
		return( BedList() );
	}
	return( _siteData.at( i ).data );
}
const	BedList	MyApp::domains( const QString& sid ) const
{
	int i = UINT;

	if( ( i = SidSetIndex<BedSet>( _domainData, sid ) ) == UINT ) {
		return( BedList() );
	}
	return( _domainData.at( i ).data );

}
const	BedList	MyApp::peaks( const QString& sid ) const
{
	int i = UINT;

	if( ( i = SidSetIndex<BedSet>( _peakData, sid ) ) == UINT ) {
		return( BedList() );
	}
	return( _peakData.at( i ).data );
}
/*
void	MyApp::QCPeakFind( const QList<int>& idxList )
{
	int	_maxValue = APP_I( "maxvalue"  );
	int	_peakMinNumProbes = I( param( "PeakMinNumProbes" )->value() );
	int	_peakMergeDistance = I( param( "PeakMergeDistance" )->value() );
	double	_peakMinPctAvg = D( param( "PeakMinPctAvg" )->value() );
	double	_peakMinPctOver = D( param( "PeakMinPctOver" )->value() );

	int	didx, pidx;

	Ofp cerr;
	cerr.open( STDERR );

	foreach( int idx, idxList ) {


		didx  = SidSetIndex<BedSet>( _domainData, _input.at( idx ).sid );
		if( didx == UINT ) {
// Why would we silently fail here
			didx = _domainData.size();
			BedSet	t;
			t.sid = _input.at( idx ).sid;
			_domainData << t;
		}
		pidx  = SidSetIndex<BedSet>( _peakData, _input.at( idx ).sid );
cerr << idx << " gets p " << pidx << endl;
		if( pidx == UINT ) {
			pidx = _peakData.size();
			BedSet	t;
			t.sid = _input.at( idx ).sid;
			_peakData << t;
		}

		WZoneList	peaks;
		double	inputAverage = 0;
		double	domainAverage = 0;
		double	valueCutoff = 0;
		WigList	input = _input[ idx ].data;
		inputAverage = WigList_average( input, UDBL, _maxValue );
		BedList dom = _domainData[ didx ].data;
// WHAT IS THIS FOR?
//		inputAverage = 2.0;
		int i = 0;

cerr << "DEBUG input has " << input.size()
 << " average = " << inputAverage << " and doms has " << dom.size() << endl;

		for( int d = 0; i < input.size() &&
		 d < dom.size(); ++d ) {
			for( ; i < input.size() &&
			 input[ i ].beg < dom[ d ].beg; ++i ){
				;
			}
			WigList	dtab;
			for( ; i < input.size() &&
			 input[ i ].beg < dom[ d ].end; ++i ){
				dtab << input[ i ];
			}

			domainAverage = dom[ d ].val;
cerr << "DEBUG collected " << dtab.size() << " average " << domainAverage << endl;

			valueCutoff = _peakMinPctAvg * domainAverage;

cerr << "DEBUG gives cuttof " << valueCutoff << endl;

			WigList		ztab;
			WZoneList	mtab;
			FormOverZones( dtab, ztab,
			 _peakMinNumProbes, valueCutoff,
			 _peakMinPctOver );
cerr << "DEBUG ztab size = " << ztab.size() << endl;
			if( ztab.size() == 0 ) {
				continue;
			}
			MergeZones( dtab, ztab, mtab,
			 _peakMinNumProbes, _peakMergeDistance );
cerr << "DEBUG mtab size = " << mtab.size() << endl;
			
			for( int z = 0; z < mtab.size(); ++z ) {
				double score = 0;
				double cnt = 0;
				for( int j = mtab[ z ].b; j < mtab[ z ].e;++j){
					score += dtab[ j ].val;
					++cnt;
				}
				if( cnt > 0 ) {
					score /= cnt;
				}
				mtab[ z ].val = score;
			}
			peaks << mtab;
		}
		_peakData[ pidx ].data.clear();
		_peakData[ pidx ].data << BedList_fromWZoneList( peaks );
		emit( peaksUpdated( _input[ idx ].sid ) );
	}
	cerr.close();
	return;
}
void	MyApp::QCDomainFind( const QList<int>& idxList )
{
	double	minScoreCalc = 0;

	if( !isGui() ) {
		cout << "RUNNING" << endl;
	}
	int	_maxValue = APP_I( "maxvalue"  );
	int	_minSeedValue = I( param( "MinSeedValue" )->value() );
	int	_minSeedNumProbes = I( param( "MinSeedNumProbes" )->value() );
	// double	_minSeedPctOver = param( "MinSeedPctOver" )->value().toDouble();
	double	_minSeedScore = D( param( "MinSeedScore" )->value() );

	double	_minCoverage = D( param( "MinCoverage" )->value() );

	int	_mergeDistance = I( param( "MergeDistance" )->value() );
	int	_minZoneSize = I( param( "MinZoneSize" )->value() );
	double	_minZoneScore = D( param( "MinZoneScore" )->value() );

	int	_xWidth = I( param( "ExtendWidth" )->value() );
	double	_xPct = D( param( "ExtendPctOver" )->value() );

	int	didx = 0;

	foreach( int idx, idxList ) {

		didx  = SidSetIndex<BedSet>( _domainData, _input.at( idx ).sid );
		if( didx == UINT ) {
			didx = _domainData.size();
			BedSet	t;
			t.sid = _input.at( idx ).sid;
			_domainData << t;
		}

		WZoneList	seeds;

		WigList	input = _input.at( idx ).data;
		double	inputAverage = WigList_average( input );

*
 *	CHANGING THIS PCT OVER THING TO A MIN SCORE (FLAT)
  	REMOVE minSeedPctOver
 *
		WigList_findSeeds( input, seeds, _minSeedNumProbes,
		 _minSeedValue, _maxValue, _minSeedScore );
emit( log( QString( "%1 seeds to start" ).arg( seeds.size() ) ) );

		WZoneList_coverageFilter( seeds, _minCoverage );
emit( log( QString( "%1 seeds after coverage" ).arg( seeds.size() ) ) );

		WZoneList_calcScore( input, seeds, UDBL, _maxValue );
		WZoneList_scoreFilter( seeds, _minSeedScore );
emit( log( QString( "%1 seeds after score" ).arg( seeds.size() ) ) );

		WZoneList_merge( seeds, _mergeDistance );
		WZoneList_minSizeFilter( seeds, _minZoneSize );
emit( log( QString( "%1 seeds after merge/size" ).arg( seeds.size() ) ) );

		WZoneList_calcScore( input, seeds, UDBL, _maxValue );
		WZoneList_extend( input, seeds, _maxValue, _xWidth, _xPct );
		WZoneList_merge( seeds, _mergeDistance );
emit( log( QString( "%1 seeds after extension" ).arg( seeds.size() ) ) );

		WZoneList_calcScore( input, seeds, UDBL, _maxValue );
		WZoneList_minSizeFilter( seeds, _minZoneSize );
		WZoneList_scoreFilter( seeds, _minZoneScore );
emit( log( QString( "%1 seeds after final score filter" ).arg( seeds.size() ) ) );
emit( log( "------------------\n\n" ) );

*
		double	inputAverage;
		PVList	input = _input[ idx ].asPVList( &inputAverage );
		_domainData[ idx ].clear();
		QList<Zone>	seeds;

		PVListFindSeeds( input, seeds, _minSeedNumProbes,
		 _minSeedValue, _maxValue, _minSeedPctOver );
cout << "DEBUG seeds " << seeds.size() << endl;
		ZoneListCalcScore( input, seeds, UVAL, _maxValue );
	int holdSize = seeds.size();
		ZoneListFilterByCoverage( seeds, 0.001 );
cout << "DEBUG coverage removed " << holdSize - seeds.size() << endl;

		minScoreCalc = ( _minSeedScore / 100 ) * inputAverage;
cout << "DEBUG " << minScoreCalc << " ( " << _minSeedScore << " / 100 ) * " << inputAverage << endl;
	holdSize = seeds.size();
		ZoneListFilterByScore( seeds, minScoreCalc );
cout << "DEBUG score filter removed " << holdSize - seeds.size() << endl;

		ZoneListDoMerge( seeds, _mergeDistance );
cout << "DEBUG merge left " << seeds.size() << endl;

		ZoneListCalcScore( input, seeds, UVAL, _maxValue );
		ZoneListFilterByMinSize( seeds, _minZoneSize );
cout << "DEBUG size filter ( " << _minZoneSize  << " ) reduced to " << seeds.size() << endl;

		ZoneListDoExtension( input, seeds, _maxValue, _xWidth, _xPct );
		ZoneListDoMerge( seeds, _mergeDistance );
		ZoneListCalcScore( input, seeds, UVAL, _maxValue );
		ZoneListFilterByMinSize( seeds, _minZoneSize );
cout << "DEBUG size filter (after-extension) ( " << _minZoneSize  << " ) reduced to " << seeds.size() << endl;

		minScoreCalc = ( _minZoneScore / 100 ) * inputAverage;
cout << "DEBUG " << minScoreCalc << " ( " << _minZoneScore << " / 100 ) * " << inputAverage << endl;
		ZoneListFilterByScore( seeds, minScoreCalc );
cout << "DEBUG final number of domains " << seeds.size() << endl;

*
		_domainData[ didx ].data.clear();
		_domainData[ didx ].data << BedList_fromWZoneList( seeds );

		emit( domainsUpdated( _input[ idx ].sid ) );
		if( !isGui() ) {
			for( int i = 0; i < seeds.size(); ++i ) {
				cout << _input[ idx ].sid << "\t"
				 << seeds[ i ].beg << "\t" << seeds[ i ].end << "\t"
				 << seeds[ i ].val << endl;
			}
		}
	}
}
*/
void	MyApp::doDomainFind()
{
	QList<int>	idxToRun;

	if( _input.size() == 0  && !loadInput() ) {
		setCritical( "Domain finding requires input first" );
		showError();
		return;
	}
	if( ( _currentInputIndex =
	 SidSetIndex<WigSet>( _input, APP_S( "currentsid" ) ) ) == UINT ) {
		setCritical( "Domain finding failed. Sid not found in input" );
		showError();
		return;
	}
	if( _input.at( _currentInputIndex ).data.size() == 0 ) {
		setCritical( "Domain finding requires input for selected sid" );
		showError();
		return;
	}
	if( APP_B( "domains/runall" ) ) {
		for( int i = 0; i < _input.size(); ++i ) {
			idxToRun << i;
		}
	} else {
		idxToRun << _currentInputIndex;
	}
	QList<DomainDol>	dolList;

	foreach( int idx, idxToRun ) {
		DomainDol	dol;
		dol.sid = _input.at( idx ).sid;

		dol.wig = _input.at( idx ).data;
		foreach( QString p, paramNames() ) {
			dol.setParam( p, param( p )->value() );
		}
		dolList << dol;
	}
	RunFuture<void>( QtConcurrent::map( dolList, &DomainDol::work ), isGui() );

	foreach( DomainDol dol, dolList ) {
		if( dol.hasError() ) {
			setError( dol.error() );
			break;
		}
	}
	if( hasError() ) {
		showError();
	} else {
		foreach( DomainDol dol, dolList ) {
			int didx = SidSetIndex<BedSet>( _domainData, dol.sid );
			if( didx == UINT ) {
				didx = _domainData.size();
				BedSet t;
				t.sid = dol.sid;
				_domainData << t;
			}
			_domainData[ didx ].data = dol.result;
			emit( domainsUpdated( dol.sid ) );
		}
	}

	//RunFuture( QtConcurrent::run( this, &MyApp::QCDomainFind, idxToRun ), isGui() );
	return;
}
void	MyApp::doPeakFind()
{
	QList<int>	idxToRun;

	if( _input.size() == 0 ) {
		setCritical( "Peak finding requires input first" );
		showError();
		return;
	}
	if( ( _currentInputIndex =
	 SidSetIndex<WigSet>( _input, APP_S( "currentsid" ) ) ) == UINT ) {
		setCritical( "Peak finding failed. Sid not found in input" );
		showError();
		return;
	}
	if( _input.at( _currentInputIndex ).data.size() == 0 ) {
		setCritical( "Peak finding requires input for selected sid" );
		showError();
		return;
	}
	if( _domainData.at( _currentInputIndex ).data.size() == 0 ) {
		setCritical( "Peak finding requires domains for selected sid" );
		showError();
		return;
	}
	if( APP_B( "peaks/runall" ) ) {
		for( int i = 0; i < _input.size(); ++i ) {
			idxToRun << i;
		}
	} else {
		idxToRun << _currentInputIndex;
	}
// BEGIN NEW
	QList<PeakDol>	dolList;

	foreach( int idx, idxToRun ) {
		PeakDol	dol;
		dol.sid = _input.at( idx ).sid;

		int didx = SidSetIndex<BedSet>( _domainData, dol.sid );

		dol.wig = _input.at( idx ).data;
		dol.dom = _domainData.at( didx ).data;
		foreach( QString p, paramNames() ) {
			dol.setParam( p, param( p )->value() );
		}
		dolList << dol;
	}
	RunFuture<void>( QtConcurrent::map( dolList, &PeakDol::work ), isGui() );

	foreach( PeakDol dol, dolList ) {
		if( dol.hasError() ) {
			setError( dol.error() );
			break;
		}
	}
	if( hasError() ) {
		showError();
	} else {
		foreach( PeakDol dol, dolList ) {
			int pidx = SidSetIndex<BedSet>( _peakData, dol.sid );
			if( pidx == UINT ) {
				pidx = _peakData.size();
				BedSet t;
				t.sid = dol.sid;
				_peakData << t;
			}
			_peakData[ pidx ].data = dol.result;
			emit( peaksUpdated( dol.sid ) );
		}
	}
// END NEW
	//RunFuture( QtConcurrent::run( this, &MyApp::QCPeakFind, idxToRun ), isGui() );
	return;
}
bool	MyApp::loadInput()
{
	WigLoader	wigLoader;

	_sidList.clear();

	if( !wigLoader.load( APP_V( "input/file" ), _input, isGui() ) ) {
		setError( wigLoader.error() );
		showError();
	} else {
		for( int i = 0; i < _input.size(); ++i ) {
			_sidList << _input.at( i ).sid;
		}
		qSort( _sidList.begin(), _sidList.end(), SortSidByChr );
	// BAD WIG INPUT CAN LEAD TO ISSUES HERE BUT THE BUG IS IN GHBIO/GHSALSA
		if( _sidList.size() > 0 ) {
			emit( inputChanged( _sidList ) );
		}
		emit( log( QString( "loaded %1 input chromosomes" )
			.arg( _sidList.size() ) ) );
	}
	return( errorState() );
}
bool	MyApp::loadDomains()
{
	BedLoader	bedLoader;

	if( !bedLoader.load( APP_V( "domains/file" ), _domainData, isGui() ) ) {
		setError( bedLoader.error() );
		showError();
	} else {
		for( int i = 0; i < _domainData.size(); ++i ) {
			emit( domainsUpdated( _domainData.at( i ).sid ) );
		}
	}
	return( errorState() );
}
bool	MyApp::loadPeaks()
{
	BedLoader	bedLoader;

	if( !bedLoader.load( APP_V( "peaks/file" ), _peakData, isGui() ) ) {
		setError( bedLoader.error() );
		showError();
	} else {
		for( int i = 0; i < _peakData.size(); ++i ) {
			emit( peaksUpdated( _peakData.at( i ).sid ) );
		}
	}
	return( errorState() );
}
// SHOULD USE BED FUNCTIONS
bool	MyApp::writeBedList( const BedList& data, const QString& path )
{
	Ofp	fp;

	if( !fp.open( path ) ) {
		return( false );
	}
	for( int i = 0; i < data.size(); ++i ) {
		fp << data.at( i ).sid;
		fp << "\t";
		fp << data.at( i ).beg;
		fp << "\t";
		fp << data.at( i ).end;
		fp << "\t";
		fp << data.at( i ).label;
		fp << "\t";
		if( data.at( i ).aux == 0 ) {
			fp << "+";
		} else {
			fp << "-";
		}
		fp << "\t";
		fp << data.at( i ).val;
		fp << "\n";
	}
	fp.close();
	return( true );
}
void	MyApp::setDomains( const QString& sid, const BedList& domains )
{
	int	_maxValue = APP_I( "maxvalue" );

	int	domIdx = UINT;
	int	idx = UINT;

	if( ( idx = SidSetIndex<WigSet>( _input, sid ) ) == UINT ) {
		qDebug() << "WARN: " << sid << " not in input";
		return;
	}
	if( ( domIdx = SidSetIndex<BedSet>( _domainData, sid ) ) == UINT ) {
		qDebug() << "WARN: " << sid << " not in domain data";
		return;
	}

qDebug() << "DOMAINS GETTING SET, SHOULD EMIT UPDATE " << _domainData[ domIdx ].data.size() << " -> " << domains.size();
	_domainData[ domIdx ].data = domains;
	for( int i = 0; i < _domainData[ domIdx ].data.size(); ++i ) {
		if( _domainData[ domIdx ].data.at( i ).val == UDBL ) {
			_domainData[ domIdx ].data[ i ].calcScore(
			  _input[ idx ].data, 0, _maxValue );
		}
	}
	emit( domainsUpdated( sid ) );
}
void	MyApp::setPeaks( const QString& sid, const BedList& peaks )
{
	int	_maxValue = APP_I( "maxvalue" );

	int	peakIdx = UINT;
	int	idx = UINT;

	if( ( idx = SidSetIndex<WigSet>( _input, sid ) ) == UINT ) {
		qDebug() << "WARN: " << sid << " not in input";
		return;
	}
	if( ( peakIdx = SidSetIndex<BedSet>( _peakData, sid ) ) == UINT ) {
		qDebug() << "WARN: " << sid << " not in domain data";
		return;
	}

	_peakData[ peakIdx ].data = peaks;
	for( int i = 0; i < _peakData[ peakIdx ].data.size(); ++i ) {
		if( _peakData[ peakIdx ].data.at( i ).val == UDBL ) {
			_peakData[ peakIdx ].data[ i ].calcScore(
			  _input[ idx ].data, 0, _maxValue );
		}
	}
	emit( peaksUpdated( sid ) );
}
// THIS LOOKS LIKE IT MIGHT CRASH
bool	MyApp::saveAllDomains( const QString& path )
{
	Q_UNUSED( path );
	return( true );
}
bool	MyApp::saveAllPeaks( const QString& path )
{
	Q_UNUSED( path );
	return( true );
}
bool	MyApp::saveDomains( const QString& sid, const QString& path )
{
	int	domIdx = UINT;

	if( ( domIdx = SidSetIndex<BedSet>( _domainData, sid ) ) == UINT ) {
		return( false );
	}
	return( saveDomains( domIdx, path ) );
}
bool	MyApp::savePeaks( const QString& sid, const QString& path )
{
	int	peakIdx = UINT;

	if( ( peakIdx = SidSetIndex<BedSet>( _peakData, sid ) ) == UINT ) {
		return( false );
	}
	return( savePeaks( peakIdx, path ) );
}
bool	MyApp::saveDomains( const int& idx, const QString& path )
{
	if( idx < 0 || idx >= _domainData.size() ) {
		return( false );
	}
	// need to make a function for handleBedOutput and
	// use it here as in solexaWizard
	return( writeBedList( _domainData[ idx ].data, path ) );
}
bool	MyApp::savePeaks( const int& idx, const QString& path )
{
	if( idx < 0 || idx >= _peakData.size() ) {
		return( false );
	}
	return( writeBedList( _peakData[ idx ].data, path ) );
}
}	//	GH namespace
