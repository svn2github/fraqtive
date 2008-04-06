/**************************************************************************
* This file is part of the Fraqtive program
* Copyright (C) 2004-2008 Michał Męciński
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
**************************************************************************/

#include "datastructures.h"

#include <QDataStream>

QDataStream& operator <<( QDataStream& stream, const FractalType& type )
{
    stream << (qint8)type.m_fractal;
    if ( type.m_fractal == JuliaFractal )
        stream << type.m_parameter;

    stream << (qint8)type.m_exponentType;
    if ( type.m_exponentType == IntegralExponent )
        stream << (qint8)type.m_integralExponent;
    else if ( type.m_exponentType == RealExponent )
        stream << type.m_realExponent;

    stream << (qint8)type.m_variant;

    return stream;
}

QDataStream& operator >>( QDataStream& stream, FractalType& type )
{
    qint8 fractal, exponentType, exponent, variant;

    stream >> fractal;
    type.m_fractal = (Fractal)fractal;
    if ( fractal == JuliaFractal )
        stream >> type.m_parameter;

    stream >> exponentType;
    type.m_exponentType = (ExponentType)exponentType;

    if ( exponentType == IntegralExponent ) {
        stream >> exponent;
        type.m_integralExponent = exponent;
    } else if ( exponentType == RealExponent ) {
        stream >> type.m_realExponent;
    }

    stream >> variant;
    type.m_variant = (GeneratorCore::Variant)variant;

    return stream;
}

QDataStream& operator <<( QDataStream& stream, const Position& position )
{
    return stream
        << position.m_center
        << position.m_zoomFactor
        << position.m_angle;
}

QDataStream& operator >>( QDataStream& stream, Position& position )
{
    return stream
        >> position.m_center
        >> position.m_zoomFactor
        >> position.m_angle;
}

QDataStream& operator <<( QDataStream& stream, const Gradient& gradient )
{
    return stream
        << gradient.m_red
        << gradient.m_green
        << gradient.m_blue;
}

QDataStream& operator >>( QDataStream& stream, Gradient& gradient )
{
    return stream
        >> gradient.m_red
        >> gradient.m_green
        >> gradient.m_blue;
}

QDataStream& operator <<( QDataStream& stream, const ColorMapping& mapping )
{
    return stream
        << mapping.m_mirrored
        << mapping.m_reversed
        << mapping.m_scale
        << mapping.m_offset;
}

QDataStream& operator >>( QDataStream& stream, ColorMapping& mapping )
{
    return stream
        >> mapping.m_mirrored
        >> mapping.m_reversed
        >> mapping.m_scale
        >> mapping.m_offset;
}

QDataStream& operator <<( QDataStream& stream, const GeneratorSettings& settings )
{
    return stream
        << settings.m_calculationDepth
        << settings.m_detailThreshold;
}

QDataStream& operator >>( QDataStream& stream, GeneratorSettings& settings )
{
    return stream
        >> settings.m_calculationDepth
        >> settings.m_detailThreshold;
}

QDataStream& operator <<( QDataStream& stream, const ViewSettings& settings )
{
    return stream << (qint8)settings.m_antiAliasing;
}

QDataStream& operator >>( QDataStream& stream, ViewSettings& settings )
{
    qint8 antiAliasing;
    stream >> antiAliasing;
    settings.m_antiAliasing = (AntiAliasing)antiAliasing;
    return stream;
}

QDataStream& operator <<( QDataStream& stream, const Bookmark& bookmark )
{
    return stream
        << bookmark.m_fractalType
        << bookmark.m_position;
}

QDataStream& operator >>( QDataStream& stream, Bookmark& bookmark )
{
    return stream
        >> bookmark.m_fractalType
        >> bookmark.m_position;
}

QDataStream& operator <<( QDataStream& stream, const Preset& preset )
{
    return stream
        << preset.m_gradient
        << preset.m_backgroundColor
        << preset.m_colorMapping;
}

QDataStream& operator >>( QDataStream& stream, Preset& preset )
{
    return stream
        >> preset.m_gradient
        >> preset.m_backgroundColor
        >> preset.m_colorMapping;
}

void registerDataStructures()
{
    qRegisterMetaTypeStreamOperators<FractalType>( "FractalType" );
    qRegisterMetaTypeStreamOperators<Position>( "Position" );
    qRegisterMetaTypeStreamOperators<Gradient>( "Gradient" );
    qRegisterMetaTypeStreamOperators<ColorMapping>( "ColorMapping" );
    qRegisterMetaTypeStreamOperators<GeneratorSettings>( "GeneratorSettings" );
    qRegisterMetaTypeStreamOperators<ViewSettings>( "ViewSettings" );
    qRegisterMetaTypeStreamOperators<Bookmark>( "Bookmark" );
    qRegisterMetaTypeStreamOperators<Preset>( "Preset" );
}
