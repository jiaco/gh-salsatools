#ifndef	MYAPP_H
#define	MYAPP_H	1
#include <GhCore.h>
#include <GhBio.h>
#include <GhGui.h>
#include <GhSalsa.h>

namespace	GH
{
class	MyApp :	public	CliApp
{
	Q_OBJECT

public:
	MyApp( int argc, char** argv  );

	void	doit();		// commandlinemode

public slots:
	void	doPreCalc();
	QImage*	doPlot();
	QImage* doImage();

	bool	loadInput();
	bool	loadFeature();

signals:
	void	preCalcFinished( const bool& tf );

protected:
	bool	validate();
	bool	doMethod();
	bool	doPixels();

	bool	runBounds();
	void	getYRange();

	WigSetList		_input;
	BedSetList		_features;
	WigList			_data, _pdata;
	double			_ymin, _ymax;
	int			_dws, _sws;
	QStringList		_inputFiles, _featureFiles;
	int			_imageWidth, _imageHeight;

	bool	_usedPreCalc;

	int	xincr;
	Axes	axes;
	Imager	*imager;

	QString	_imagePath;

	Ofp	cerr, cout;
};
}	//	GH namespace
#endif	//	MYAPP_H
