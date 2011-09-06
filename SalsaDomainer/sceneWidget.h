#ifndef	SCENEWIDGET_H
#define	SCENEWIDGET_H	1
#include <GhGui.h>
#include <GhSalsa.h>
#include "myApp.h"

namespace	GH
{
const	int	GITEM_TYPE = 0;
const	int	GITEM_INDEX = 1;

const	int	GITEM_DOMAIN = 1;
const	int	GITEM_PEAK = 2;
const	int	GITEM_SITE = 3;

const	qreal	DEFAULT_WIDTH = 600;
const	qreal	DEFAULT_HEIGHT = 600;
const	qreal	DEFAULT_MARGIN = 10;
const	qreal	DEFAULT_RES = 50;

//
//	as long as this class is here, no export needed
//
//class	GH_DLL_EXPORT	SceneWidget	:	public	QWidget
class	SceneWidget	:	public	QWidget
{
	Q_OBJECT

public:
	SceneWidget( const QString& title, MyApp* app, QWidget* parent = 0 );

	void	updateGraphics();
/*
	void	updateGraphics(
		 const qreal& res = DEFAULT_RES,
		 const qreal& width = DEFAULT_WIDTH,
		 const qreal& height = DEFAULT_HEIGHT,
		 const qreal& margin = DEFAULT_MARGIN
		);
*/
	void	setupInput();
	void	redrawInput();
	void	redrawSites();
	void	redrawDomains();
	void	redrawPeaks();

	void	redoBedList( const int& item_type, const BedList& data,
		 const QColor& color, QList<QGraphicsRectItem*>& itab,
		 TableWidget* table, const QStringList& columns );

signals:
	// need to connect an error message to window.cpp
	//
	void	saveDomains( const QString& sid, const QString& path );
	void	savePeaks( const QString& sid, const QString& path );
	void	domainsAltered( const QString& sid, const BedList& domains );
	void	peaksAltered( const QString& sid, const BedList& domains );

	void	message( const QString& text,
		 const int& type = QMessageBox::Critical );
public slots:
	void	tableSiteSelect( const int& row, const int& col );
	void	tableDomainSelect( const int& row, const int& col );
	void	tablePeakSelect( const int& row, const int& col );
	void	viewItemClicked( QGraphicsItem* item );

	void	callOpenDomainInBrowser();
	void	callOpenPeakInBrowser();
	void	openUcscTableSelection( TableWidget* table,
		 const QString& sid, const BedList& data );
	void	openUcscUrl( const QString& sid,
		 const int& bed, const int& end );
	void	callSaveDomains();
	void	callSavePeaks();
	void	callDeleteDomains();
	void	callMergeDomains();
	void	callDeletePeaks();
	void	callMergePeaks();
	void	redraw();

private:
	MyApp	*app;
	QString	_sid;

	QList<int>	getSelectedRows( QTableWidget* table );

	QList<QGraphicsRectItem*>	domlist, peaklist, sitelist;
	void	highlightDomain( const int& index );
	void	highlightPeak( const int& index );

	QSplitter	*_splitter;
	QTabWidget	*_tabWidget;
	QWidget		*_tabDomains, *_tabPeaks, *_controlWidget;

	TableWidget	*tableDomains, *tablePeaks, *tableSites;

	QAction	*_actSaveDomains, *_actSavePeaks;
	QAction	*_actDeleteDomains, *_actMergeDomains;
	QAction	*_actDeletePeaks, *_actMergePeaks;

	QAction	*_actOpenDomainInBrowser, *_actOpenPeakInBrowser;

	QLineEdit	*EditResolution;

	Scene	*scene;
	View	*view;
	QColor	clrInput, clrDomains, clrPeaks, clrSites, clrSelected;
	QPen	pen;
	QBrush	brush;
	qreal	_width, _height;
	qreal	_margin, _res, g_x0, g_y0, g_xmax, g_ymax;
	qreal	d_xmax, d_ymax;
	qreal	p_maxValue;
};
}	//	GH namespace
#endif	//	SCENEWIDGET_H
