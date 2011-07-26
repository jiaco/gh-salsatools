#include "dol.h"
namespace	GH
{
	DomainDol::DomainDol()
{
}
	PeakDol::PeakDol()
{
}
void	DomainDol::work()
{
	double	minScoreCalc, inputAverage;

	int	maxValue = I( getParam( "MaxValue" ) );
	int	minSeedValue = I( getParam( "MinSeedValue" ) );
	int	minSeedNumProbes = I( getParam( "MinSeedNumProbes" ) );
	double	minSeedScore = D( getParam( "MinSeedScore" ) );
	
	double	minCoverage = D( getParam( "MinCoverage" ) );

	int	mergeDistance = I( getParam( "MergeDistance" ) );
	int	minZoneSize = I( getParam( "MinZoneSize" ) );
	double	minZoneScore = D( getParam( "MinZoneScore" ) );

	int	xWidth = I( getParam( "ExtendWidth" ) );
	double	xPct = D( getParam( "ExtendPctOver" ) );

	WZoneList	seeds;

	minScoreCalc = 0;
	result.clear();

	inputAverage = WigList_average( wig );
	WigList_findSeeds( wig, seeds, minSeedNumProbes,
	 minSeedValue, maxValue, minSeedScore );
	WZoneList_coverageFilter( seeds, minCoverage );
	WZoneList_calcScore( wig, seeds, UDBL, maxValue );
	WZoneList_scoreFilter( seeds, minSeedScore );
	WZoneList_merge( seeds, mergeDistance );
	WZoneList_minSizeFilter( seeds, minZoneSize );
	WZoneList_calcScore( wig, seeds, UDBL, maxValue );
	WZoneList_extend( wig, seeds, maxValue, xWidth, xPct );
	WZoneList_merge( seeds, mergeDistance );
	WZoneList_calcScore( wig, seeds, UDBL, maxValue );
	WZoneList_minSizeFilter( seeds, minZoneSize );
	WZoneList_scoreFilter( seeds, minZoneScore );

	result = BedList_fromWZoneList( seeds );
}
void	PeakDol::work()
{
	double	inputAverage, domainAverage, valueCutoff;
	int maxValue = I( getParam( "MaxValue" ) );
	int peakMergeDistance = I( getParam( "PeakMergeDistance" ) );
	double peakMinNumProbes = I( getParam( "PeakMinNumProbes" ) );
	double peakMinPctAvg = D( getParam( "PeakMinPctAvg" ) );
	double peakMinPctOver = D( getParam( "PeakMinPctOver" ) );

	WZoneList	mtab, peaks;
	QList<Wig>	dtab, ztab;

	result.clear();
	inputAverage = domainAverage = valueCutoff = 0;

	inputAverage = WigList_average( wig, UDBL, maxValue );
	int i = 0;
	for( int d = 0; i < wig.size() && d < dom.size(); ++d ) {
		for(; i < wig.size() && wig.at( i ).beg < dom.at( d ).beg; ++i){
			;
		}
		dtab.clear();
		for(; i < wig.size() && wig.at( i ).beg < dom.at( d ).end; ++i){
			dtab << wig.at( i );
		}
		domainAverage = dom.at( d ).val;
		valueCutoff = peakMinPctAvg * domainAverage;

		ztab.clear();
		mtab.clear();
		FormOverZones( dtab, ztab, peakMinNumProbes, valueCutoff,
		 peakMinPctOver );
		if( ztab.isEmpty() ) {
			continue;
		}
		MergeZones( dtab, ztab, mtab, peakMinNumProbes,
		 peakMergeDistance );
		for( int z = 0; z < mtab.size(); ++z ) {
			double score, cnt;
			score = cnt = 0;
			for( int j = mtab[ z ].b; j < mtab[ z ].e; ++j ) {
				score += dtab.at( j ).val;
				++cnt;
			}
			if( cnt > 0 ) {
				score /= cnt;
			}
			mtab[ z ].val = score;
		}
		peaks << mtab;
	}
	result = BedList_fromWZoneList( peaks );
}
}	//	GH namespace
