#ifndef	OPERATIONS_H
#define	OPERATIONS_H	1
#include <QtGui>
#include <GhGui.h>
#include <GhSalsa.h>

namespace	GH
{
//typedef	QList<PV>	PVList;

GH_DLL_EXPORT	PVList	QCBounds( const QList<PVContainer>& input,
		const QList<Bed>& doms, const int& dws, const int& sws );
GH_DLL_EXPORT	PVList	Bounds( const QList<PVContainer>& input,
		const QList<Bed>& doms, const int& dws, const int& sws,
		 const bool& showDialog = false );

/*
 *	turns out we do not need this but will need
 * 	to be able to filter the tss features based on domains
 *
GH_DLL_EXPORT	PVList	QCGlob( const QList<PVContainer>& input,
		const QList<Bed>& doms, const int& dws, const int& sws );
GH_DLL_EXPORT	PVList	Glob( const QList<PVContainer>& input,
		const QList<Bed>& doms, const int& dws, const int& sws,
		 const bool& showDialog = false );
*/


GH_DLL_EXPORT	PVList	Smooth( const PVList& data,
		 const int& dataSize, const int& windowSize,
		 const int& minPosition, const int& incPosition );

GH_DLL_EXPORT	QImage	RenderScene( Scene* scene, const int& width, const int& height );

GH_DLL_EXPORT	QImage*	Plot( const PVList& data,
		const int& dws, const int& sws,
		const QString& imageTitle = "",
		const int& width = 600, const int& height = 600 );


}	//	GH namespace
#endif	//	OPERATIONS_H
