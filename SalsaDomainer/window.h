#ifndef	WINDOW_H
#define	WINDOW_H	1
#include <GhGui.h>
#include "myApp.h"
#include "sceneWidget.h"
#include "ghGuiCommon.h"

namespace	GH
{
class	Window	:	public	MainWindow
{
	Q_OBJECT

public:
	Window( MyApp* app = 0 );

public slots:
	void	activateSceneWidget( const QString& sid );
	void	dispatchUpdate( const QString& sid );
signals:
	void	log( const QString& text );

private:
	MyApp		*_myApp;

	QTabWidget	*_tabWidget;
	QWidget		*_widgetInput, *_widgetDomains, *_widgetPeaks;
	QWidget		*_widgetCentral;

	QPushButton	*_btn_colorInput, *_btn_colorDomain, *_btn_colorPeak;
	QColor		_colorInput, _colorDomain, _colorPeak;

	QMap<QString,SceneWidget*>	swmap;
};
}	//	GH namespace
#endif	//	WINDOW_H
