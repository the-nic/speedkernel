/*
SpeedSim - a OGame (www.ogame.org) combat simulator
Copyright (C) 2004-2007 Maximialian Matthé & Nicolas Höft

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

#include "pnggraph.h"

CPngGraph::CPngGraph(int height, int width)
{
    m_PngData.settext("SpeedSim Graph", "Nicolas Höft", "Ship Information", "SpeedSim");
    m_PngData.resize(width, height);
    m_Height = height;
    m_Width = width;
    m_BackgrndColor = sColor(65535, 65535, 65535);
    m_AxisColor = sColor(0, 0, 0);
    m_TextColor = sColor(0, 0, 0);
    m_GraphColor = sColor(65535, 0, 0);
}

CPngGraph::~CPngGraph()
{
}

void CPngGraph::SetColours(sColor BackgrndColor, sColor GraphColor, sColor AxisColor, sColor TextColor)
{
    m_BackgrndColor = BackgrndColor;
    m_GraphColor = GraphColor;
    m_TextColor = TextColor;
}

bool CPngGraph::CreateAxes(float xMin, float xMax, float yMin, float yMax, float xStep, float yStep)
{
    if ((xMin == xMax) || (yMin == yMax))
        return false;
    if (xMin > xMax)
    {
        float tmp = xMin;
        xMin = xMax;
        xMax = tmp;
    }
    if (yMin > yMax)
    {
        float tmp = yMin;
        yMin = yMax;
        yMax = tmp;
    }
    if(xStep == 0)
        xStep = 1;
    if(yStep == 0)
        yStep = 1;
    m_minX = xMin;
    m_maxX = xMax;
    // erase old image data
    m_PngData.filledsquare(0, 0, m_Width, m_Height, m_BackgrndColor.r, m_BackgrndColor.g, m_BackgrndColor.b);

    float length = abs(xMin - xMax);
    float height = abs(yMin - yMax);
	length += length * 0.1f;
	height += height * 0.1f;
    m_pixPerX = m_Width * 0.91f / length;
    m_pixPerY = m_Height * 0.91f / height;;
    
    // set orgin of coordinate system
    if (xMin * 0.95f >= 0)
        m_Orgin.x = m_Width * 0.05f;
    else if (xMax * 1.05f <= 0)
        m_Orgin.x = m_Width * 0.95f;
    else
        m_Orgin.x = abs(m_Width * xMin / length);
    
    if (yMin * 0.95f >= 0)
        m_Orgin.y = m_Height * 0.05f;
    else if (yMax * 1.05f <= 0)
        m_Orgin.y = m_Height * 0.95f;
    else
        m_Orgin.y = abs(m_Height * yMin / height);
    
    // x-axis
    m_PngData.arrow(m_Width * 0.03f, m_Orgin.y, m_Width * 0.99f, m_Orgin.y, 10, 0.6, m_AxisColor.r, 
        m_AxisColor.g, m_AxisColor.b);
    // y-axis
    m_PngData.arrow(m_Orgin.x , m_Height * 0.03f, m_Orgin.x, m_Height * 0.99f, 10, 0.6, m_AxisColor.r, 
        m_AxisColor.g, m_AxisColor.b);
	

    sPoint tmpOrg = m_Orgin;
	// now move orgin to 'correct' position
	if (xMin > 0)
        m_Orgin.x -= m_pixPerX * xMin;
    else if (xMax < 0)
        m_Orgin.x += m_pixPerX * xMax;

	if (yMin > 0)
        m_Orgin.y -= m_pixPerY * yMin;
    else if (yMax < 0)
        m_Orgin.y += m_pixPerY * yMax;

    float pos = xMin;
    bool bMoveNum = false;
    while(pos <= xMax)
    {
        if(int(pos * 100) == 0) {
            pos += xStep;
            continue;
        }
        int xPos = pos * m_pixPerX + m_Orgin.x;
        m_PngData.line(xPos , tmpOrg.y - 5, xPos, tmpOrg.y + 5, m_AxisColor.r, m_AxisColor.g, m_AxisColor.b);
        char c[10];
        sprintf(c, "%.0f", pos);
        int yPos = tmpOrg.y - 15;
        if(bMoveNum)
            yPos -= 7;
        bMoveNum = !bMoveNum;
        m_PngData.plot_text("cour.ttf", 8, xPos - 5, yPos, 0, c, 
            m_TextColor.r, m_TextColor.g, m_TextColor.b);
        pos += xStep;
    }
    pos = yMin;
    while(pos <= yMax)
    {
        if(int(pos * 100) == 0) {
            pos += yStep;
            continue;
        }
        int yPos = pos * m_pixPerY + m_Orgin.y;
        m_PngData.line(tmpOrg.x - 5, yPos, tmpOrg.x + 5, yPos, m_AxisColor.r, m_AxisColor.g, m_AxisColor.b);
        char c[10];
        sprintf(c, "%.1f", pos);
        m_PngData.plot_text("cour.ttf", 8, tmpOrg.x + 8, yPos, 0, c, 
            m_TextColor.r, m_TextColor.g, m_TextColor.b);
        pos += yStep;
    }
    m_bAxesDone = true;
    return true;
}

bool CPngGraph::WriteToFile(char *filename)
{
    m_PngData.pngwriter_rename(filename);
    m_PngData.write_png();
    return true;
}

int CPngGraph::PlotData(vector<sPoint> data, unsigned long flags)
{
    int nPoints = 0;
    if(!data.size())
        return 0;
    PrepareData(data);
    sPoint lastp(data[0].x * m_pixPerX + m_Orgin.x, data[0].y * m_pixPerY + m_Orgin.y);
    for (size_t i = 0; i < data.size(); i++)
    {
        sPoint p;
        p.x = data[i].x * m_pixPerX + m_Orgin.x;
        p.y = data[i].y * m_pixPerY + m_Orgin.y;
        if(p.x < 0 || p.y < 0)
            continue;
        nPoints++;
        if(((flags & PLOT_DOT)^PLOT_DOT) == 0)
        {
            //m_PngData.circle(p.x, p.y, 1, m_GraphColor.r, m_GraphColor.g, m_GraphColor.b);
            m_PngData.plot(p.x, p.y, m_GraphColor.r, m_GraphColor.g, m_GraphColor.b);
        }
        else if(((flags & PLOT_CIRCLE)^PLOT_CIRCLE) == 0)
        {
            m_PngData.filledcircle(p.x, p.y, 3, m_GraphColor.r, m_GraphColor.g, m_GraphColor.b);
        }
        else if(((flags & PLOT_CROSS)^PLOT_CROSS) == 0)
        {
            m_PngData.cross(p.x, p.y, 2, 2, m_GraphColor.r, m_GraphColor.g, m_GraphColor.b);
        }
        if(((flags & PLOT_CONNECT_POINTS)^PLOT_CONNECT_POINTS) == 0 || ((flags & PLOT_FILL)^PLOT_FILL) == 0)
        {
            m_PngData.line(lastp.x, lastp.y, p.x, p.y, m_GraphColor.r, m_GraphColor.g, m_GraphColor.b);
        }
        lastp = p;
    }

    // fill data
    //if(data

    return nPoints;
}

void CPngGraph::PrepareData(vector<sPoint> &points)
{
    if(points.size() < 3)
        return;
    bool bCheckDone = false;
	sort(points.begin(), points.end());
    while(!bCheckDone)
    {
        bCheckDone = true;
        // sort out all too data that is less the min of painted axis
        vector<sPoint>::iterator it = points.begin() + 1;
        if(it->x < m_minX) {
            points.erase(it);
            bCheckDone = false;
        }
        // data that is greater then xmax
        it = points.end() - 1;
        if(it->x > m_maxX) {
            points.erase(it);
            bCheckDone = false;
        }
    }
}
