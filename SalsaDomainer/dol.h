#ifndef	DOL_H
#define	DOL_H	1
#include "GhCore.h"
#include "GhBio.h"

namespace	GH
{
class	DomainDol	:	public	DacBase
{
public:
	DomainDol();

	QList<Wig>	wig;
	QList<Bed>	result;

	void	work();

	QString	sid;
};

class	PeakDol	:	public	DacBase
{
public:
	PeakDol();

	QList<Wig>	wig;
	QList<Bed>	dom;
	QList<Bed>	result;

	void	work();

	QString	sid;
};

}	//	GH namespace
#endif	//	DOL_H
