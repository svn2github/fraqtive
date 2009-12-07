/**************************************************************************
* This file is part of the Fraqtive program
* Copyright (C) 2004-2008 Michał Męciński
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
**************************************************************************/

#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

class QColor;
class QTransform;

class FractalData;
class Gradient;
class ColorMapping;
class ViewSettings;
class AnimationState;

class AbstractView
{
public:
    AbstractView() { }
    virtual ~AbstractView() { }

public:
    virtual void clearView() = 0;

    virtual void transformView( const QTransform& transform ) = 0;

    virtual void initialUpdate( const FractalData* data ) = 0;
    virtual void partialUpdate( const FractalData* data ) = 0;
    virtual void fullUpdate( const FractalData* data ) = 0;

    virtual void setColorSettings( const Gradient& gradient, const QColor& backgroundColor, const ColorMapping& mapping ) = 0;
    virtual void setGradient( const Gradient& gradient ) = 0;
    virtual void setBackgroundColor( const QColor& color ) = 0;
    virtual void setColorMapping( const ColorMapping& mapping ) = 0;

    virtual void setViewSettings( const ViewSettings& settings ) = 0;

    virtual void setAnimationState( const AnimationState& state ) = 0;
};

#endif
