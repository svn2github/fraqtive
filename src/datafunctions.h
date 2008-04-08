/**************************************************************************
* This file is part of the Fraqtive program
* Copyright (C) 2004-2008 Michał Męciński
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
**************************************************************************/

#ifndef DATAFUNCTIONS_H
#define DATAFUNCTIONS_H

#include <QGradient>

#include "datastructures.h"
#include "generatorcore.h"

class FractalData;

namespace DataFunctions
{

Position defaultPosition( const FractalType& type );

Gradient defaultGradient();
ColorMapping defaultColorMapping();

GeneratorSettings defaultGeneratorSettings();
ViewSettings defaultViewSettings();

QGradientStops calculateGradientStops( const Gradient& gradient );

void fillGradientCache( const Gradient& gradient, QRgb* cache, int size );

class ColorMapper
{
public:
    ColorMapper( const QRgb* gradientCache, int gradientSize, QRgb backgroundColor, const ColorMapping& mapping );

public:
    QRgb map( double value ) const;

private:
    const QRgb* m_gradientCache;
    int m_gradientSize;

    QRgb m_backgroundColor;

    ColorMapping m_mapping;
};

void drawImage( QImage& image, const FractalData* data, const QRect& region, const ColorMapper& mapper, AntiAliasing antiAliasing );

GeneratorCore::Functor* createFunctor( const FractalType& type );

#if defined( HAVE_SSE2 )

GeneratorCore::FunctorSSE2* createFunctorSSE2( const FractalType& type );

#endif

} // namespace DataFunctions

#endif
