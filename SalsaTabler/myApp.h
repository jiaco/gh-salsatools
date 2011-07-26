#ifndef	MYAPP_H
#define	MYAPP_H	1
#include <GhCore.h>
#include <GhBio.h>
#include <GhGui.h>
#include <GhSalsa.h>
#include "dol.h"

namespace	GH
{
const	QString	HEADER = "sid,beg,end,strand,gene";

class	MyApp	:	public	CliApp
{
	Q_OBJECT

public:
	MyApp( int argc, char** argv );

	//TableModel	*getModel();

	QStringList		tableHeader() const;
	QList<QList<QVariant> >	tableData() const;

public slots:
	bool	doit();
	bool	loadBedInput();
	bool	loadWigInput();
	bool	updateTable();
	bool	addToTable();

	void	deleteColumn( const QList<int>& cols );
	bool	dumpToFile( QTextStream& fp );
	bool	openInExcel();
signals:
	void	newTableData();
	void	log( const QString& s );

private:
	//TableModel		*tableModel;
	QList<QList<QVariant> >	_tableData;
	QStringList		_tableHeader;

	BedSetList	_bedInput;
	WigSetList	_wigInput;
	int		_inputColumnCount;

	QList<Dol>	data;
	Ofp	cout, cerr;
};

}	//	GH namespace
#endif	//	MYAPP_H
