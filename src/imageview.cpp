/**************************************************************************
* This file is part of the Fraqtive program
* Copyright (C) 2004-2008 Michał Męciński
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
**************************************************************************/

#include "imageview.h"

#include <math.h>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include "fractalpresenter.h"
#include "datafunctions.h"

ImageView::ImageView( QWidget* parent, FractalPresenter* presenter ) : QWidget( parent ),
    m_presenter( presenter ),
    m_interactive( false ),
    m_gradientCache( NULL ),
    m_tracking( NoTracking )
{
    setContextMenuPolicy( Qt::PreventContextMenu );
}

ImageView::~ImageView()
{
    delete[] m_gradientCache;
}

void ImageView::setInteractive( bool interactive )
{
    m_interactive = interactive;

    if ( interactive ) {
        setFocusPolicy( Qt::WheelFocus );
        setMouseTracking( true );
    } else {
        setFocusPolicy( Qt::NoFocus );
        setMouseTracking( false );
    }
}

void ImageView::clearView()
{
    m_image = QImage();
    m_updatedRegion = QRect();
    m_tracking = NoTracking;

    if ( m_interactive ) {
        m_presenter->clearTracking();
        m_presenter->clearHovering();
    }

    update();
}

void ImageView::transformView( const QMatrix& transform )
{
    if ( m_image.isNull() )
        return;

    QImage image( size(), QImage::Format_RGB32 );
    image.fill( m_backgroundColor.rgb() );

    QPainter painter( &image );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setWorldMatrix( transform * m_scale );
    painter.drawImage( 0, 0, m_image );

    m_image = image;
    m_updatedRegion = QRect();

    calculateScale();
    update();
}

void ImageView::initialUpdate( const FractalData* data )
{
    if ( m_image.isNull() || m_image.size() != data->size() - QSize( 2, 2 ) ) {
        QImage image( data->size() - QSize( 2, 2 ), QImage::Format_RGB32 );
        image.fill( m_backgroundColor.rgb() );

        if ( !m_image.isNull() ) {
            QPainter painter( &image );
            painter.setRenderHint( QPainter::SmoothPixmapTransform );
            painter.setWorldMatrix( m_scale );
            painter.drawImage( 0, 0, m_image );
        }

        m_image = image;

        calculateScale();
        update();
    }

    m_updatedRegion = QRect();
    partialUpdate( data );
}

void ImageView::partialUpdate( const FractalData* data )
{
    const QList<QRect> validRegions = data->validRegions();

    if ( validRegions.count() > 0 && validRegions.first().top() == 0 && validRegions.first().bottom() > m_updatedRegion.bottom() ) {
        int top = qMax( m_updatedRegion.bottom() - 1, 0 );
        QRect region( 0, top, validRegions.first().width() - 2, validRegions.first().bottom() - top - 1 ); 
        drawImage( data, region );
        m_updatedRegion = validRegions.first();

        update( worldMatrix().mapRect( region ).adjusted( -1, -1, 1, 1 ) );
    }
}

void ImageView::fullUpdate( const FractalData* data )
{
    m_image = QImage( data->size() - QSize( 2, 2 ), QImage::Format_RGB32 );
    drawImage( data, m_image.rect() );

    m_updatedRegion = m_image.rect();

    calculateScale();
    update();
}

static const int GradientSize = 16384;

void ImageView::drawImage( const FractalData* data, const QRect& region )
{
    DataFunctions::ColorMapper mapper( m_gradientCache, GradientSize, m_backgroundColor.rgb(), m_colorMapping );
    DataFunctions::drawImage( m_image, data, region, mapper, m_settings.antiAliasing() );
}

void ImageView::setColorSettings( const Gradient& gradient, const QColor& backgroundColor, const ColorMapping& mapping )
{
    m_gradient = gradient;
    m_backgroundColor = backgroundColor;
    m_colorMapping = mapping;

    updateGradient();
    updateBackground();
    updateImage();
}

void ImageView::setGradient( const Gradient& gradient )
{
    m_gradient = gradient;

    updateGradient();
    updateImage();
}

void ImageView::setBackgroundColor( const QColor& backgroundColor )
{
    m_backgroundColor = backgroundColor;

    updateBackground();
    updateImage();
}

void ImageView::setColorMapping( const ColorMapping& mapping )
{
    m_colorMapping = mapping;

    updateImage();
}

void ImageView::setViewSettings( const ViewSettings& settings )
{
    m_settings = settings;

    updateImage();
}

void ImageView::updateGradient()
{
    if ( !m_gradientCache )
        m_gradientCache = new QRgb[ GradientSize ];

    DataFunctions::fillGradientCache( m_gradient, m_gradientCache, GradientSize );
}

void ImageView::updateBackground()
{
    QPalette palette;
    palette.setBrush( QPalette::Background, m_backgroundColor );

    setPalette( palette );
    setBackgroundRole( QPalette::Background );
    setAutoFillBackground( true );
}

void ImageView::updateImage()
{
    if ( m_image.isNull() )
        return;

    const FractalData* data = m_presenter->fractalData();

    if ( m_image.size() != data->size() - QSize( 2, 2 ) )
        return;

    m_updatedRegion = QRect();
    partialUpdate( data );
}

void ImageView::resizeEvent( QResizeEvent* e )
{
    m_presenter->setResolution( e->size() + QSize( 2, 2 ) );
    calculateScale();
}

void ImageView::calculateScale()
{
    double scale = (double)height() / (double)m_image.height();
    QPointF center( width() / 2.0, height() / 2.0 );
    QPointF imageCenter( m_image.width() / 2.0, m_image.height() / 2.0 );

    // scale image to view size and move it to the center of the view
    m_scale.reset();
    m_scale.translate( center.x(), center.y() );
    m_scale.scale( scale, scale );
    m_scale.translate( -imageCenter.x(), -imageCenter.y() );

    m_invScale = m_scale.inverted();
}

void ImageView::paintEvent( QPaintEvent* /*e*/ )
{
    if ( m_image.isNull() )
        return;

    QPainter painter( this );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setWorldMatrix( worldMatrix() );
    painter.drawImage( 0, 0, m_image );
}

QMatrix ImageView::worldMatrix()
{
    if ( m_tracking != NoTracking )
        return m_transform * m_scale;
    else
        return m_scale;
}

void ImageView::mousePressEvent( QMouseEvent* e )
{
    if ( !m_interactive || m_image.isNull() )
        return;

    if ( m_tracking != NoTracking ) {
        m_tracking = NoTracking;
        m_presenter->clearTracking();
        if ( !m_transform.isIdentity() )
            update();
        return;
    }

    m_trackStart = e->pos();

    if ( e->button() == Qt::LeftButton ) {
        if ( e->modifiers() & Qt::ShiftModifier )
            m_tracking = DragMove;
        else if ( e->modifiers() & Qt::ControlModifier )
            m_tracking = ZoomCenter;
        else
            m_tracking = DragZoomIn;
    } else if ( e->button() == Qt::RightButton ) {
        if ( e->modifiers() & Qt::ShiftModifier )
            m_tracking = DragRotate;
        else if ( e->modifiers() & Qt::ControlModifier )
            m_tracking = RotateCenter;
        else
            m_tracking = DragZoomOut;
    } else if ( e->button() == Qt::MidButton ) {
        m_tracking = DragMove;
    }

    m_transform.reset();
}

void ImageView::mouseMoveEvent( QMouseEvent* e )
{
    if ( !m_interactive || m_image.isNull() )
        return;

    if ( m_tracking == NoTracking ) {
        QPointF point = worldMatrix().inverted().map( e->pos() );
        m_presenter->setHoveringPoint( point );
        return;
    }

    const double zoomFactor = 400.0;
    const double rotateFactor = 2.0;

    QPointF center( width() / 2.0, height() / 2.0 );
    QLineF offset( m_trackStart, e->pos() );

    QMatrix matrix;

    // first calculate new transformation in view coordinates
    switch ( m_tracking ) {
        case DragMove:
            matrix.translate( offset.dx(), offset.dy() );
            break;

        case ZoomCenter: {
            double zoom = pow( 10.0, offset.dy() / zoomFactor );
            matrix.translate( center.x(), center.y() );
            matrix.scale( zoom, zoom );
            matrix.translate( -center.x(), -center.y() );
            break;
        }

        case DragZoomIn:
        case DragZoomOut: {
            double direction = ( m_tracking == DragZoomIn ) ? 1.0 : -1.0;
            double zoom = pow( 10.0, direction * offset.length() / zoomFactor );
            matrix.translate( m_trackStart.x(), m_trackStart.y() );
            matrix.scale( zoom, zoom );
            matrix.translate( -m_trackStart.x(), -m_trackStart.y() );
            break;
            }

        case DragRotate: {
            QLineF from( center, m_trackStart );
            QLineF to( center, e->pos() );
            double fromAngle = atan2( from.dy(), from.dx() );
            double toAngle = atan2( to.dy(), to.dx() );
            matrix.translate( center.x(), center.y() );
            matrix.rotate( ( toAngle - fromAngle ) * 180.0 / M_PI );
            matrix.translate( -center.x(), -center.y() );
            break;
        }

        case RotateCenter: {
            double angle = offset.dx() / rotateFactor;
            matrix.translate( center.x(), center.y() );
            matrix.rotate( angle );
            matrix.translate( -center.x(), -center.y() );
            break;
        }
    }

    // then convert it from view coordinates to image coordinates
    m_transform = m_scale * matrix * m_invScale;
    m_presenter->setTrackingTransform( m_transform );

    update();
}

void ImageView::mouseReleaseEvent( QMouseEvent* /*e*/ )
{
    if ( !m_interactive || m_tracking == NoTracking || m_image.isNull() )
        return;

    if ( !m_transform.isIdentity() ) {
        m_presenter->changePosition( m_transform );
        update();
    }

    m_tracking = NoTracking;
    m_presenter->clearTracking();
}

void ImageView::mouseDoubleClickEvent( QMouseEvent* e )
{
    if ( m_interactive && !m_image.isNull() ) {
        QPointF point = worldMatrix().inverted().map( e->pos() );
        m_presenter->switchToJulia( point );
    }
}

void ImageView::wheelEvent( QWheelEvent* e )
{
    if ( !m_interactive || m_image.isNull() || m_tracking != NoTracking ) {
        e->ignore();
        return;
    }

    Tracking mode = NoTracking;
    if ( e->modifiers() & Qt::ShiftModifier )
        mode = RotateCenter;
    else if ( e->modifiers() & Qt::ControlModifier )
        mode = ZoomCenter;
    else
        mode = ZoomPoint;

    const double zoomFactor = 120.0;
    const double rotateFactor = 0.5;

    QPointF center( width() / 2.0, height() / 2.0 );
    double delta = e->delta() / 8.0;

    QMatrix matrix;

    switch ( mode ) {
        case ZoomPoint: {
            double zoom = pow( 10.0, delta / zoomFactor );
            matrix.translate( e->pos().x(), e->pos().y() );
            matrix.scale( zoom, zoom );
            matrix.translate( -e->pos().x(), -e->pos().y() );
            break;
        }

        case ZoomCenter: {
            double zoom = pow( 10.0, delta / zoomFactor );
            matrix.translate( center.x(), center.y() );
            matrix.scale( zoom, zoom );
            matrix.translate( -center.x(), -center.y() );
            break;
        }

        case RotateCenter: {
            double angle = delta / rotateFactor;
            matrix.translate( center.x(), center.y() );
            matrix.rotate( angle );
            matrix.translate( -center.x(), -center.y() );
            break;
        }
    }

    m_presenter->changePosition( m_scale * matrix * m_invScale );

    e->accept();
}

void ImageView::leaveEvent( QEvent* /*e*/ )
{
    if ( m_interactive && !m_image.isNull() )
        m_presenter->clearHovering();
}

void ImageView::keyPressEvent( QKeyEvent* e )
{
    if ( !m_interactive || m_image.isNull() || ( e->modifiers() & Qt::AltModifier ) != 0 ) {
        e->ignore();
        return;
    }

    if ( e->key() == Qt::Key_Escape && m_tracking != NoTracking ) {
        e->accept();
        m_tracking = NoTracking;
        m_presenter->clearTracking();
        if ( !m_transform.isIdentity() )
            update();
        return;
    }

    QPointF center( width() / 2.0, height() / 2.0 );
    double step = height() / 4.0;

    QMatrix matrix;

    switch ( e->key() ) {
        case Qt::Key_Up:
            matrix.translate( 0, step );
            break;

        case Qt::Key_Down:
            matrix.translate( 0, -step );
            break;

        case Qt::Key_Left:
            matrix.translate( step, 0 );
            break;

        case Qt::Key_Right:
            matrix.translate( -step, 0 );
            break;

        case Qt::Key_Plus:
        case Qt::Key_Equal:
            matrix.translate( center.x(), center.y() );
            matrix.scale( 2.0, 2.0 );
            matrix.translate( -center.x(), -center.y() );
            break;

        case Qt::Key_Minus:
            matrix.translate( center.x(), center.y() );
            matrix.scale( 0.5, 0.5 );
            matrix.translate( -center.x(), -center.y() );
            break;
    }

    if ( !matrix.isIdentity() ) {
        e->accept();
        m_presenter->changePosition( m_scale * matrix * m_invScale );
    } else {
        e->ignore();
    }
}
