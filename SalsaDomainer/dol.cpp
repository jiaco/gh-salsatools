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

	int	maxValue = APP_I( "maxvalue" );
	int	minSeedValue = APP_I( "domains/minseedvalue" );
	int	minSeedNumProbes = APP_I( "domains/minseednumprobes" );
	double	minSeedScore = APP_D( "domains/minseedscore" );
	
	double	minCoverage = APP_D( "domains/mincoverage" );

	int	mergeDistance = APP_I( "domains/mergedistance" );
	int	minZoneSize = APP_I( "domains/minzonesize" );
	double	minZoneScore = APP_D( "domains/minzonescore" );

	int	xWidth = APP_I( "domains/extendwidth" );
	double	xPct = APP_D( "domains/extendpctover" );

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
	int maxValue = APP_I( "maxvalue" );
	int peakMergeDistance = APP_I( "peaks/mergedistance" );
	double peakMinNumProbes = APP_I( "peaks/minnumprobes" );
	double peakMinPctAvg = APP_D( "peaks/minpctavg" );
	double peakMinPctOver = APP_D( "peaks/minpctover" );

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
