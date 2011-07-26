#include "dol.h"


using namespace	GH;

	Dol::Dol()
	 : DacBase()
{
	basicFeatures << "UpZone" << "Zone" << "DnZone";
	allFeatures = basicFeatures;
	allFeatures << "Exon" << "Intron";

	allCalculations
	 << "MeanOnCount"
	 << "MeanOnLength"
	 << "Median"
	 << "Variance"
	 << "Count";

	foreach( QString s, basicFeatures ) {
		features.insert( s, s );
		upParam.insert( s, QString( "%1_%2" )
			.arg( s ).arg( "UpDist" ) );
		dnParam.insert( s, QString( "%1_%2" )
			.arg( s ).arg( "DnDist" ) );
	}
	foreach( QString s, allFeatures ) {
		if( !basicFeatures.contains( s ) ) {
			features.insert( s, s );
			upParam.insert( s, QString( "%1_%2" )
				.arg( s ).arg( "UpDist" ) );
			dnParam.insert( s, QString( "%1_%2" )
				.arg( s ).arg( "DnDist" ) );
		}
	}
}
QList<double>	Dol::subsetWigValues( const int& lo, const int& hi )
{
	//
	//	Error-free as an empty return value will safely pass
	//	 through to runCalculations->doCalculation->ghMath
	//	 and return appropriate zero-result
	//
	QList<double>	rv;

	if( hi <= lo ) {
		return( rv );
	}
	for( int i = 0; i < wig.size(); ++i ) {
		if( wig.at( i ).beg < lo ) {
			continue;
		}
		if( wig.at( i ).beg > hi ) {
			break;
		}
		rv << wig.at( i ).val;
	}
	return( rv );
}
int	Dol::runCalculations( const int& row, const QList<double>& values,
		const QStringList& calcTypes, const int& colStart, const int& opt )
{
	int	col = colStart;
	foreach( QString calc, calcTypes ) {
		data[ row ][ col ] = doCalculation( values, calc );
		if( calc == "MeanOnLength" && opt > 0 ) {
			data[ row ][ col ] /= opt;
		}
		++col;
	}
	return( col );
}
double	Dol::doCalculation( const QList<double>& values, const QString& calcKey )
{
	double	rv = 0;

	if( calcKey == "MeanOnCount" ) {
		rv = Mean<double>( values );
	} else if( calcKey == "MeanOnLength" ) {
		rv = Sum<double>( values );
		// divide by length implemented in the call above
	} else if( calcKey == "Median" ) {
		rv = Median<double>( values );
	} else if( calcKey == "Variance" ) {
		rv = Variance<double>( values );
	} else if( calcKey == "Count" ) {
		rv = values.size();
	}
	return( rv );
}
void	Dol::work()
{
	// need to expand header to handle multiple calculations per run
	// there are these possible Zones:
	//	UpZone Zone DnZone Exon Intron
	// and there are these possible Calcs:
	//	MeanOnCount MeanOnLength Median Variance Count
	//
	// if everything is selected then 5 * 5 = 25 columns to add
	//
	QString	wigLabel = "NA";
	QStringList	doCalc;

	wigLabel = "NA";

	if( hasParam( "WigLabel" ) ) {
		wigLabel = S( getParam( "WigLabel" ) );
	}

	/*
		each param is set in MyApp constructor and they are inited 
		so we should not have to check hasParam for each bool check below
	 */
	hdr.clear();
	foreach( QString feat, allFeatures ) {
		if( !B( getParam( features[ feat ] ) ) ) {
			continue;
		}
		foreach( QString calc, allCalculations ) {
			if( !B( getParam( calc ) ) ) {
				continue;
			}
		// this looks suspicious, we must be in this logic too many times
		// and yet if we do a calc for one feat type, we do it for all...
		//
			if( !doCalc.contains( calc ) ) {
				doCalc << calc;
			}
			hdr << QString( "%1-%2-%3" )
				.arg( wigLabel )
				.arg( feat )
				.arg( calc );
		}
	}
	// zero out everything so we always have the right size
	// myApp depends on this as tableData nrow is not checked
	//
	for( int i = 0; i < bed.size(); ++i ) {
		QList<double>	tdl;
		for( int j = 0; j < hdr.size(); ++j ) {
			tdl << 0;
		}
		data << tdl;
	}



	int	lo, hi, blo, bhi, aux, col, up, dn;
	QList<double>	vset;
	blo = bhi = 0;

	for( int i = 0; i < bed.size(); ++i ) {
		aux = bed[ i ].aux;
		col = 0;
		foreach( QString f, basicFeatures ) {
			if( !B( getParam( features[ f ] ) ) ) {
				continue;
			}
			if( ( f == "UpZone" && aux == 0 ) ||
			 ( f == "DnZone" && aux == 1 ) ) {
				blo = bed[ i ].beg;
				bhi = bed[ i ].beg;
			} else if( ( f == "DnZone" && aux == 0 ) ||
			 ( f == "UpZone" && aux == 1 ) ) {
				blo = bed[ i ].end;
				bhi = bed[ i ].end;
			} else if( f == "Zone" ) {
				blo = bed[ i ].beg;
				bhi = bed[ i ].end;
			}
			up = I( getParam( upParam[ f ] ) );
			dn = I( getParam( dnParam[ f ] ) );
			if( aux == 0 ) {
				lo = blo - up;
				hi = bhi + dn;
			} else {
				lo = blo - dn;
				hi = bhi + up;
			}
			vset = WigSubsetValues( wig, lo, hi );
			col = runCalculations( i, vset, doCalc, col, hi - lo ); 
/*
 * can do it all in one, but with stats still to do, maybe not...

			col = runCalculations( i,
				WigSubsetValues( wig, lo, hi ),
				doCalc, col, hi - lo );
*/
		// OLD WAY
		//	vset = subsetWigValues( lo, hi );
/*
			foreach( QString calc, doCalc ) {
				if( calc == "MeanOnLength" ) {
					data[ i ][ col ] = Sum<double>( vset ) / ( hi - lo );
				} else {
					data[ i ][ col ] = doCalculation( vset, calc );
				}

				// Feed stats obj here giving the vset

				++col;
			}
*/
		}
		// need to inc col if do exon or do intron
		QString f = "Exon";
	
		if( B( getParam( features[ f ] ) ) && bed[ i ].sub != 0 ) {
			vset.clear();
			int	featureLength = 0;
			for( int x = 0; x < bed[ i ].sub->beg.size(); ++x ) {
				up = I( getParam( upParam[ f ] ) );
				dn = I( getParam( dnParam[ f ] ) );
				if( aux == 0 ) {
					lo = bed[ i ].sub->beg.at( x ) - up;
					hi = bed[ i ].sub->end.at( x ) + dn;
				} else {
					lo = bed[ i ].sub->beg.at( x ) - dn;
					hi = bed[ i ].sub->end.at( x ) + up;
				}
				if( hi > lo ) {
					featureLength += ( hi - lo );
					vset << WigSubsetValues( wig, lo, hi );
				}
			}
			col = runCalculations( i, vset, doCalc, col, featureLength ); 
/*
			foreach( QString calc, doCalc ) {
				data[ i ][ col ] = doCalculation( vset, calc );

				// Feed stats obj here giving the vset

				++col;
			}
*/
		}
		f = "Intron";
		if( B( getParam( features[ f ] ) ) && bed[ i ].sub != 0 ) {
			vset.clear();
			int	featureLength = 0;
			for( int x = 1; x < bed[ i ].sub->beg.size(); ++x ) {
				up = I( getParam( upParam[ f ] ) );
				dn = I( getParam( dnParam[ f ] ) );
				if( aux == 0 ) {
					lo = bed[ i ].sub->end.at( x-1 ) - up;
					hi = bed[ i ].sub->beg.at( x ) + dn;
				} else {
					lo = bed[ i ].sub->end.at( x-1 ) - dn;
					hi = bed[ i ].sub->beg.at( x ) + up;
				}
				if( hi > lo ) {
					featureLength += hi - lo;
					vset << WigSubsetValues( wig, lo, hi );
				}
			}
			col = runCalculations( i, vset, doCalc, col, featureLength ); 
/*
			foreach( QString calc, doCalc ) {
				data[ i ][ col ] = doCalculation( vset, calc );

				// Feed stats obj here giving the vset

				++col;
			}
*/
		}
	}
	return;
}
