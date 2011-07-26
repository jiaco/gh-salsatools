TEMPLATE=app
TGT="SalsaTabler"

CONFIG += gh bio gui salsa

#RESOURCES += $$(GH_HOME)/resources/headers.qrc

HEADERS +=\
	window.h	\
	dol.h	\
	myApp.h

SOURCES +=\
	window.cpp	\
	myApp.cpp	\
	dol.cpp	\
	_main.cpp

include( $$(GH_HOME)/ghbase.pri )
