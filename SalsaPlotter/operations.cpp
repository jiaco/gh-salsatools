#include "operations.h"
#include <QDebug>

namespace	GH
{

PVList	QCBounds( const QList<PVContainer>& input,
		const QList<Bed>& doms,
		const int& dws, const int& sws )
{
	PVList	rv;
	PV	t;
	int	rpos, rp1, rp2, aux;
	int	dead = 10*dws - 1;

	for( int i = 0; i < input.size(); ++i ) {
		BedList	chrDoms;
		bool	hasStrand = false;
		for( int j = 0; j < doms.size(); ++j ) {
			if( doms[ j ].sid == input[ i ].sid ) {
				chrDoms << doms[ j ];
				if( doms[ j ].aux != -1 ) {
					hasStrand = true;
				}
			}
		}	
qDebug() << "Bounding " << input[ i ].sid << " " << input[ i ].pos.size() << " " << chrDoms.size();
if( hasStrand ) {
	qDebug() << "has strand is true";
}
		for( int j = 0; j < input[ i ].pos.size(); ++j ) {
			for( int k = 0; k < chrDoms.size(); ++k ) {
				rp1 = rp2 = dead;
				if( hasStrand ) {
					if( chrDoms[ k ].aux == 0 ) {
						rp1 = input[ i ].pos[ j ] - chrDoms[ k ].beg;
					} else {
						rp1 = chrDoms[ k ].end - input[ i ].pos[ j ];
					}
				} else {
					rp1 = input[ i ].pos[ j ] - chrDoms[ k ].beg;
					rp2 = chrDoms[ k ].end - input[ i ].pos[ j ];
					//rpos = input[ i ].pos[ j ] - chrDoms[ k ].beg;
				}
				if( rp1 != dead && qAbs( rp1 ) <= dws + sws ) {
					t.pos = rp1;
/*
					if( hasStrand && chrDoms[ k ].aux == 1 ) {
						t.pos *= -1;
					}
*/
					t.val = input[ i ].val[ j ];
					rv << t;
				}
				if( rp2 != dead && qAbs( rp2 ) <= dws + sws ) {
					t.pos = rp2;
/* should not happen
					if( hasStrand && chrDoms[ k ].aux == 1 ) {
						t.pos *= -1;
					}
*/
					t.val = input[ i ].val[ j ];
					rv << t;
				}
				
/*
				if( qAbs( rpos ) <= dws + sws ) {
					t.pos = rpos;
					t.val = input[ i ].val[ j ];
					rv << t;
				}
				rpos = chrDoms[ k ].end - input[ i ].pos[ j ];
				if( qAbs( rpos ) <= dws + sws ) {
					t.pos = rpos;
					t.val = input[ i ].val[ j ];
					rv << t;
				}
*/
			}
		}
	}
	qSort( rv.begin(), rv.end(), PVSort );
	return( rv );
}
PVList	Bounds( const QList<PVContainer>& input,
			const QList<Bed>& doms,
			const int& dws, const int& sws,
			const bool& showDialog )
{
	QFuture<PVList>	f;
	QFutureWatcher<PVList>	fw;
	FutureDialog	*dialog = 0;

	if( showDialog ) {
		dialog = new FutureDialog();
		dialog->setLabelText(
		 QString( "Processing %1 things with %2 thread(s)" )
		 .arg( input.size() ).arg( QThread::idealThreadCount() ) );
		dialog->attach( &fw );
	}
	f = QtConcurrent::run( QCBounds, input, doms, dws, sws );
	fw.setFuture( f );
	if( showDialog ) {
		dialog->exec();
	}
	fw.waitForFinished();
	PVList	rv;
	for( int i = 0; i < f.resultCount(); ++i ) {
		rv += f.resultAt( i );
	}
	if( dialog ) {
		delete dialog;
	}
	return( rv );
}
PVList	Smooth( const PVList& data,
		 const int& dataSize, const int& windowSize,
		 const int& minPosition, const int& incPosition )
{
	int	lastIdx = 0;
	PVList	rv;
	for( int i = 0; i < dataSize; ++i ) {
		int lo = minPosition + ( i * incPosition );
		int hi = lo + windowSize;
		PV p;
		
		p.pos = i;
		p.val = 0;
		int	 cnt = 0;
		for( int idx = lastIdx; idx < data.size(); ++idx ) {
			if( data[ idx ].pos < lo ) {
				lastIdx = idx;
				continue;
			}
			if( data[ idx ].pos > hi ) {
				break;
			}
			p.val += data[ idx ].val;
			++cnt;
		}
		p.val /= cnt;
		rv << p;
	}
	return( rv );
}
QImage	RenderScene( Scene* scene, const int& width, const int& height )
{
qDebug() << "DEBUG  1";
	QImage rv( width, height, QImage::Format_ARGB32_Premultiplied );
qDebug() << "DEBUG  2";
	//rv.fill( 0 );
	QPainter painter( &rv );
qDebug() << "DEBUG  3";
	scene->render( &painter );
	//delete painter;
qDebug() << "DEBUG  4";
	return( rv );
}
QImage*	Plot( const PVList& data, const int& dws, const int& sws,
	const QString& imageTitle,
	const int& width, const int& height )
{
	Axes		axes;
	Imager		*imager = new Imager( width, height );
	Scene		*scene = new Scene();

	PVList	pdata;

	axes.xinit( ( dws * -1 ), dws );

	int	xincr;
	if( ( xincr = axes.xrange / imager->xspace ) <= 0 ) {
		xincr = 1;
	}
	pdata = Smooth( data, imager->xspace, sws, axes.xmin, xincr );

	axes.yinit( pdata );
	imager->initScene( scene );
	imager->setGraphicFont( FONT_AXIS, "Courier", 10 );
	imager->setGraphicFont( FONT_TITLE, "Arial", 12 );
	imager->setTitle( scene, imageTitle );
	if( !imager->plotData( scene, axes, pdata, "black" ) ) {
		return( 0 );
		//return( QImage() );
	}
	imager->colorRightSide( scene, "lightgrey" );
	
        QList<qreal>    xlegend;
        QList<qreal>    ylegend;

        int     nleg = 4;
        qreal legincr = ( axes.ymax - axes.ymin ) / nleg;
        for( qreal leg = axes.ymin; leg <= axes.ymax; leg += legincr ) {
                ylegend << leg;
        }
        if( ylegend.size() < nleg + 1 ) {
                ylegend << axes.ymax;
        }
        legincr = ( axes.xmax - axes.xmin ) / nleg;
        for( int leg = axes.xmin; leg <= axes.xmax; leg += legincr ) {
                xlegend << leg;
        }
        if( xlegend.size() < nleg + 1 ) {
                xlegend << axes.xmax;
        }
        imager->legendX( scene, axes, 30, 10, xlegend );
        imager->legendY( scene, axes, 30, 10, ylegend );

	QImage* image = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
	QPainter* painter = new QPainter( image );
	scene->render( painter );
	while( painter->isActive() ) {
		;
	}
	delete painter;
	delete scene;
	delete imager;
	return( image );
/*
qDebug() << "RENDERING IMAGE";
	QImage image = RenderScene( scene, width, height );
	//delete imager;
	//delete scene;
qDebug() << "RETURNING IMAGE";
	return( image );
*/
}
}	//	GH namespace
