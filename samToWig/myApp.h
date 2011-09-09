#ifndef	MYAPP_H
#define	MYAPP_H	1
#include <GhCore.h>
#include <GhGui.h>

namespace	GH
{

const	int	MIN_TOK	= 10;
const	int	SID_AT = 2;
const	int	BEG_AT = 3;
const	int	SEQ_AT = 9;
class	MyApp	:	public	CliApp
{
	Q_OBJECT

public:
	MyApp( int argc, char** argv );

public slots:
	void	doit();
	
	void	dump( QTextStream& fp, const QString& sid,
		 QVector<int>& tab );
	void	dumpD( QTextStream& fp, const QString& sid,
		 QVector<int>& tab );

private:
	Ofp	cout, cerr;
};

}	//	GH namespace
#endif	//	MYAPP_H
