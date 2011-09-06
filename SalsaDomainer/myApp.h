#ifndef	MYAPP_H
#define	MYAPP_H	1
#include <GhCore.h>
#include <GhBio.h>
#include <GhGui.h>
#include <GhSalsa.h>
#include "dol.h"

namespace	GH
{
class	MyApp	:	public CliApp
{
	Q_OBJECT

public:
	MyApp( int argc, char** argv );

	void	setDefaults();
	void	showParams();
	void	doit();	// cmdline
	//void	QCDomainFind( const QList<int>& idxList );
	//void	QCPeakFind( const QList<int>& idxList );

	const	QStringList	sidList() const;

	const	BedList	sites( const QString& sid ) const;
	const	BedList	domains( const QString& sid ) const;
	const	BedList	peaks( const QString& sid ) const;
	const	WigList	input( const QString& sid ) const;

public slots:
	bool	loadInput();
	bool	loadDomains();
	bool	loadPeaks();
	void	doDomainFind();
	void	doPeakFind();

	bool	saveAllDomains( const QString& path );
	bool	saveDomains( const int& idx, const QString& path );
	bool	saveDomains( const QString& sid, const QString& path );
	bool	saveAllPeaks( const QString& path );
	bool	savePeaks( const int& idx, const QString& path );
	bool	savePeaks( const QString& sid, const QString& path );

	void	setDomains( const QString& sid, const BedList& domains );
	void	setPeaks( const QString& sid, const BedList& peaks );

signals:
	void	inputChanged( const QStringList& value );
	void	domainsUpdated( const QString& sid );
	void	peaksUpdated( const QString& sid );

	void	log( const QString& text );
private:
	
	bool	writeBedList( const BedList& data, const QString& path );
	int	_currentInputIndex;


	int	_inputMaxPos;
	double	_seedAverage, _inputMaxVal;

	QString			_outputLabel;
	QStringList		_inputFiles, _domainFiles, _peakFiles;

	WigSetList		_input;
	BedSetList		_domainData, _peakData, _siteData;
	//BedSetList		_domains, _peaks;

	/*
	 *	_chrList is a QResource from _static/human_chrlist.txt
	 * 	used to ensure proper indexing of everythin
 	 *
	 *	_sidList is the list of actual chrs loaded
	 */
	QStringList		_sidList;
	//, _chrList;
	//int			_dataSize;
	Ofp	cout;
};

}	//	GH namespace
#endif	//	MYAPP_H
