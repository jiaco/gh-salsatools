TEMPLATE=app
TGT="SalsaDomainer"

CONFIG += gh bio gui salsa

RESOURCES +=\
	application.qrc

HEADERS +=\
	dol.h	\
	sceneWidget.h	\
	window.h	\
	myApp.h

SOURCES +=\
	dol.cpp	\
	sceneWidget.cpp	\
	window.cpp	\
	myApp.cpp	\
	_main.cpp

include( $$(GH_HOME)/ghbase.pri )
