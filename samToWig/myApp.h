#ifndef	MYAPP_H
#define	MYAPP_H	1
#include <GhCore.h>
#include <GhBio.h>
#include <GhGui.h>

namespace	GH
{

const	int	MIN_TOK	= 10;
const	int	SID_AT = 2;
const	int	BEG_AT = 3;
const	int	SEQ_AT = 9;

const	QString	SPLITSORT = "splitsort";
const	QString	WIGOUT = "wigout";
const	QString	EXPORT2BAM = "exportToBam";
const	QString	EXPORT2BED = "exportToBed";
const	QString	BAM2BED = "bamToBed";

const	int	EXPORT_SEQ = 8;
const	int	EXPORT_QUAL = 9;
const	int	EXPORT_SID = 10;
const	int	EXPORT_XXX = 11;
const	int	EXPORT_BEG = 12;
const	int	EXPORT_AUX = 13;
const	int	EXPORT_XD_Z = 14;
const	int	EXPORT_SM_I = 15;

class	MyApp	:	public	CliApp
{
	Q_OBJECT

public:
	MyApp( int argc, char** argv );

	enum	InputFormat
		{
			Undef, Bam, Export
		};

public slots:
	bool	doit();
	
/*
	void	dump( QTextStream& fp, const QString& sid,
		 QVector<int>& tab );
	void	dumpD( QTextStream& fp, const QString& sid,
		 QVector<int>& tab );
*/
	bool	readyInput();
	bool	exportToBam();
	bool	exportToBed();
	bool	bamToBed();
	bool	wigOut();
	bool	splitSort();

protected:
	QString	formIdFromExport( const Token& tok ) const;
private:
	Ofp	cout, cerr;

	Ifp	ifp;
	QString	_inputFile, _outputDir, _outputBase;
	InputFormat	_inputFormat;
	QStringList	_runChoices;
};

}	//	GH namespace
#endif	//	MYAPP_H
