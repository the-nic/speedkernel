/*
SpeedSim - a OGame (www.ogame.org) combat simulator
Copyright (C) 2004-2006 Maximialian Matthé & Nicolas Höft

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef PNGGRAPH_H
#define PNGGRAPH_H

#include "pngwriter.h"
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

#define PLOT_DOT 0x002
#define PLOT_CIRCLE 0x004
#define PLOT_CROSS 0x008
#define PLOT_CONNECT_POINTS 0x020
#define PLOT_FILL 0x040


struct sColor 
{
    int r, g, b;
    sColor() { r = g = b = 0; }
    sColor(int red, int green, int blue) { r = red; g = green; b = blue; }
};

struct sPoint 
{
    float x, y;
    sPoint() { x = y = 0; }
    sPoint(float xPos, float yPos) { x = xPos; y = yPos;  }
    sPoint operator+(const sPoint& p) { return sPoint(x + p.x, y + p.y); };
    sPoint operator+=(const sPoint& p) { x += p.x; y += p.y; return *this; };
    sPoint operator-(const sPoint& p) { return sPoint(x - p.x, y - p.y); };
    sPoint operator-=(const sPoint& p) { x -= p.x; y -= p.y; return *this; };
    sPoint operator*(const float f) { return sPoint(x * f, y * f); };
    sPoint operator*=(const float f) { x *= f; y *= f; return *this; };
    sPoint operator/(const float f) { return sPoint(x / f, y / f); };
    sPoint operator/=(const float f) { x /= f; y /= f; return *this; };
    bool operator==(const sPoint& p) { return (x == p.x) && (y == p.y); };
    bool operator<(const sPoint& p) { return (x < p.x) && !(*this == p); }
    bool operator>(const sPoint& p) { return !(*this < p) && !(*this == p);}
};

class CPngGraph 
{
public:
    CPngGraph(int height, int width);
    ~CPngGraph();
    
    bool WriteToFile(char* filename);
    void SetColours(sColor BackgrndColor, sColor GraphColor, sColor AxisColor, sColor TextColor);

    // paints the axes including , image data will be deleted
    bool CreateAxes(float xMin, float xMax, float yMin, float yMax, float xStep, float yStep);    
    int PlotData(vector<sPoint> data, unsigned long flags);
private:
	void PrepareData(vector<sPoint> &points);

    pngwriter m_PngData;
    bool m_bAxesDone;
    
    sColor m_BackgrndColor, m_AxisColor, m_TextColor, m_GraphColor;

    int m_Height, m_Width;
    float m_pixPerX, m_pixPerY;
    float m_minX, m_maxX;
    sPoint m_Orgin;
};

#endif