#ifndef	DOL_H
#define	DOL_H	1
#include <GhCore.h>
#include <GhBio.h>
#include "ghMath.h"

namespace	GH
{

//	through DacBase get _params
//	 and Errorprone/Error
//
class	Dol	:	public	DacBase	// Division Of Labor
{
public:
	Dol();
	QList<Bed>	bed;
	QList<Wig>	wig;
	QString	sid;
	
	QStringList		hdr;
	QList<QList<double> >	data;

	void	work();

	QMap<QString,QString>	features, upParam, dnParam;
	QStringList		basicFeatures;
	QStringList		allFeatures, allCalculations;

private:
	int		runCalculations( const int& row,
			 const QList<double>& values,
			 const QStringList& calcTypes,
			 const int& colStart,
			 const int& opt = 1 );
	double		doCalculation( const QList<double>& values,
			 const QString& calcKey );
	QList<double>	subsetWigValues( const int& lo, const int& hi );
};
}	//	GH namespace
#endif	//	DOL_H
