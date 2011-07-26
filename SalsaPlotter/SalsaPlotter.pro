TEMPLATE=app
TGT="SalsaPlotter"

CONFIG += gh bio gui salsa

HEADERS +=\
	window.h	\
	myApp.h

SOURCES +=\
	window.cpp	\
	myApp.cpp	\
	_main.cpp

include( $$(GH_HOME)/ghbase.pri )
