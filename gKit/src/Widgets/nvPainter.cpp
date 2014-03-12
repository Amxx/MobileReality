
#include <cstdio>

#include "Widgets/nvWidgets.h"


namespace nv {

void UIPainter::drawFrame(const Rect& r, int margin, int style)
{
    drawRoundedRectOutline(
        Rect(r.x - margin, r.y - margin, r.w + 2*margin, r.h + 2*margin), 
        nv::Point(margin, margin), 
        cOutline);
}

Rect UIPainter::getPanelRect(const Rect& r, const Rect& text, Rect& rt, Rect& ra) const
{
    Rect rect = r;
    rt.x = getWidgetMargin();
    rt.y = getWidgetMargin();

    if (rect.h == 0) {
        rt.h = text.h;
        rect.h = rt.h + 2 * rt.y;
    } else {
        rt.h = rect.h - 2 * rt.y;
    }

    ra.h = rt.h;
    ra.w = ra.h;
    ra.y = rt.y;

    if (rect.w == 0) {
        rt.w = text.w;
        rect.w = rt.w + 2 * rt.x;
        // Add room for drop down button
        rect.w += ra.h + rt.x;
    } else {
        // Add room for drop down button
        rt.w = rect.w - 3 * rt.x - ra.h;
    }
    ra.x = 2 * rt.x + rt.w;

    return rect;
}

void UIPainter::drawPanel(const Rect& r, const char *text, const Rect& rt, const Rect& ra, bool isUnfold, bool isHover, bool isFocus, int style)
{
    drawFrame( r, Point(rt.x, rt.y), isHover, false, isFocus );
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text );
    
    if (isUnfold)
        drawMinus( Rect(r.x + ra.x, r.y + ra.y, ra.w, ra.h), int(ra.h * 0.15f), 
            cBase + (!isHover) + (isFocus << 2), cOutline);
    else
        drawPlus( Rect(r.x + ra.x, r.y + ra.y, ra.w, ra.h), int(ra.h * 0.15f), 
            cBase + (!isHover) + (isFocus << 2), cOutline);
}

Rect UIPainter::getGraphRect( const Rect& r, const int *values, const int n, Rect& rg ) const
{
    Rect rect = r;
    rg.x = getWidgetMargin();
    rg.y = getWidgetMargin();

    rg.w= n;

    if (rect.w == 0) {
        rect.w = rg.w + 2 * rg.x;
    } else {
        rg.w = rect.w - 2 * rg.x;
    }
    if (rect.h == 0) {
        rg.h = 64;
        rect.h = rg.h + 2 * rg.y;
    } else {
        rg.h = rect.h - 2 * rg.y;
    }
    
    return rect;  
}

void UIPainter::drawGraph( const Rect& r, const int *values, const int n, const Rect& rg )
{
    drawFrame( r, Point( rg.x, rg.y ), false, false, false );
    
}

Rect UIPainter::getTimebarRect( const Rect& r, const Rect& text, Rect& rt, const float start, const float stop, Rect& rb )
{
    Rect rect= r;
    if (rect.w == 0)
        rect.w = getAutoWidth() + text.w + 3 * getWidgetMargin();
    if (rect.h == 0)
        rect.h = text.h;
    
    rt.x= getWidgetMargin();
    rt.y= getWidgetMargin();
    rt.w= text.w,
    rt.h= text.h;
    float w= rect.w - text.w - 3 * getWidgetMargin();
    //~ rb.x= 2 * getWidgetMargin() + text.w + start * w;
    rb.x= 2 * getWidgetMargin() + text.w;
    rb.y= getWidgetMargin();
    //~ rb.w= std::max(2.f, (stop - start) * w);
    rb.w= w;
    rb.h= text.h;
    
    return rect;
}

void UIPainter::drawTimebar( const Rect& r, const char *text, const Rect& rt, const float start, const float stop, const Rect& rb )
{
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text );

    drawRect( Rect(r.x + rb.x, r.y + rb.y, rb.w, rb.h), cBase, cOutline );
    drawRect( Rect(r.x + rb.x + start * rb.w, r.y + rb.y, std::max(2.f, (stop - start) * rb.w), rb.h - 4), cOutline, cOutline );
}


Rect UIPainter::getLabelRect(const Rect& r, const Rect& text, Rect& rt ) const
{
    Rect rect = r;
    rt.x = getWidgetMargin();
    rt.y = getWidgetMargin();

    rt.w= text.w;

    if (rect.w == 0) {
        rect.w = rt.w + 2 * rt.x;
    } else {
        rt.w = rect.w - 2 * rt.x;
    }
    if (rect.h == 0) {
        rt.h = text.h;
        rect.h = rt.h + 2 * rt.y;
    } else {
        rt.h = rect.h - 2 * rt.y;
    }
    
    return rect;
}

void UIPainter::drawLabel(const Rect& r, const char *text, const Rect& rt, bool isHover, int style)
{
    if (style > 0 )
        drawFrame( r, Point( rt.x, rt.y ), false, false, false );
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text );
}

Rect UIPainter::getButtonRect(const Rect& r, const Rect& text, Rect& rt) const
{
    Rect rect = r;
    rt.x = getWidgetMargin();
    rt.y = getWidgetMargin();

    if (rect.w == 0) {
        rt.w = text.w;
        rect.w = rt.w + 2 * rt.x;
    } else {
        rt.w = rect.w - 2 * rt.x;
    }
    if (rect.h == 0) {
        rt.h = text.h;
        rect.h = rt.h + 2 * rt.y;
    } else {
        rt.h = rect.h - 2 * rt.y;
    }
    return rect;
}

void UIPainter::drawButton(const Rect& r, const char *text, const Rect& rt, bool isDown, bool isHover, bool isFocus, int style)
{
    drawFrame( r, Point( rt.x, rt.y ), isHover, isDown, isFocus );
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text );
}

Rect UIPainter::getCheckRect(const Rect& r, const Rect& text, Rect& rt, Rect& rc) const
{
    Rect rect = r;

    int rcOffset = (int) (0.125 * getAutoHeight());
    rc.h = getAutoHeight() - 2 * rcOffset;
    rc.w = rc.h;

    rc.x = getWidgetMargin() + rcOffset;
    rc.y = getWidgetMargin() + rcOffset;

    rt.x = getAutoHeight() + 2 * getWidgetMargin();
    rt.y = getWidgetMargin();

    if (rect.w == 0) {
        rt.w = text.w;
        rect.w = rt.x + rt.w + getWidgetMargin();
    }

    if (rect.h == 0) {
        rt.h = text.h;
        rect.h = rt.h + 2 * rt.y;
    }

    return rect;
}

void UIPainter::drawCheckButton(const Rect& r, const char *text, const Rect& rt, const Rect& rc, bool isChecked, bool isHover, bool isFocus, int style)
{
    if (style)
        drawFrame( r, Point( rt.y, rt.y ), isHover, false, isFocus );
    drawBoolFrame( Rect(r.x + rc.x, r.y + rc.y, rc.w, rc.h), Point( rc.w / 6, rc.h / 6 ), isHover, isChecked, false );
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text );
}

Rect UIPainter::getRadioRect(const Rect& r, const Rect& text, Rect& rt, Rect& rr) const
{
    Rect rect = r;

    int rrOffset = (int) (0.125 * getAutoHeight());
    rr.h = getAutoHeight() - 2 * rrOffset;
    rr.w = rr.h;

    rr.x = getWidgetMargin() + rrOffset;
    rr.y = getWidgetMargin() + rrOffset;

    rt.x = getAutoHeight() + 2 * getWidgetMargin();
    rt.y = getWidgetMargin();

    if (rect.w == 0) {
        rt.w = text.w;
        rect.w = rt.w + rt.x + getWidgetMargin();
    }

    if (rect.h == 0) {
        rt.h = text.h;
        rect.h = rt.h + 2 * rt.y;
    }

    return rect;
}

void UIPainter::drawRadioButton(const Rect& r, const char *text, const Rect& rt, const Rect& rr, bool isOn, bool isHover, bool isFocus, int style)
{
    if (style) 
        drawFrame( r, Point( rt.y, rt.y ), isHover, false, isFocus );
    drawBoolFrame( Rect(r.x + rr.x, r.y + rr.y, rr.w, rr.h), Point( rr.w / 2, rr.h / 2 ), isHover, isOn, false );
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text );
}

Rect UIPainter::getItemRect(const Rect& r, const Rect& text, Rect& rt) const
{
    Rect rect = r;
    rt.x = 0;
    rt.y = 0;

    if (rect.w == 0) {
        rt.w = text.w;
        rect.w = rt.w + 2 * rt.x;
    } else {
        rt.w = rect.w - 2 * rt.x;
    }
    if (rect.h == 0) {
        rt.h = text.h;
        rect.h = rt.h + 2 * rt.y;
    } else {
        rt.h = rect.h - 2 * rt.y;
    }
    
    return rect;
}

void UIPainter::drawListItem(const Rect& r, const char *text, const Rect& rt, bool isSelected, bool isHover, int style)
{
    // drawFrame( r, Point(0, 0), isHover, isSelected, false );
    //~ drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text, isHover, isSelected);
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text, 1, -1, isHover, isSelected);
}


Rect UIPainter::getListRect(const Rect& r, int numOptions, const Rect& options, Rect& ri, Rect& rt) const
{
    Rect rect = r;
    ri.x = getWidgetMargin();
    ri.y = getWidgetMargin();
    rt.x = getWidgetMargin();
    rt.y = getWidgetMargin();

    if (rect.w == 0) {
        rt= options;
        ri.w = rt.w + 2 * rt.x;
        rect.w = ri.w + 2 * ri.x;
    } else {
        ri.w = rect.w - 2 * ri.x;
        rt.w = ri.w - 2 * rt.x;
    }
    if (rect.h == 0) {
        rt.h = options.h / numOptions;
        ri.h = rt.h + rt.y;
        rect.h = numOptions * ri.h + 2 * ri.y;
    } else {
        ri.h = (rect.h - 2 * ri.y) / numOptions;
        rt.h = ri.h - rt.y;
    }
    
    return rect;
}

void UIPainter::drawListBox(const Rect& r, int numOptions, const char * options[], const Rect& ri, const Rect& rt, int selected, int hovered, int style)
{
    drawFrame( r, Point(ri.x, ri.y) );

    Rect ir( r.x + ri.x, r.y + r.h - ri.y - ri.h, ri.w, ri.h );
    for ( int i = 0; i < numOptions; i++ ) {
        if ( (i == hovered) || (i == selected)) {
            drawFrame( ir, Point(ri.x, ri.y), false, (i == selected));
        }

        drawText( Rect(ir.x + rt.x , ir.y + rt.y, rt.w, rt.h), options[i] );
        ir.y -= ir.h;
    }
}

Rect UIPainter::getComboRect(const Rect& r, int numOptions, const Rect& options, int selected, Rect& rt, Rect& rd) const
{
    Rect rect = r;
    rt.x = getWidgetMargin();
    rt.y = getWidgetMargin();

    if (rect.h == 0) {
        rt.h = options.h / numOptions;
        rect.h = rt.h + 2 * rt.y;
    } else {
        rt.h = rect.h - 2 * rt.y;
    }

    rd.h = rt.h;
    rd.w = rd.h;
    rd.y = rt.y;

    if (rect.w == 0) {
        //~ rt= options;
        rt.w= options.w;
        rect.w = rt.w + 2 * rt.x;

        //Add room for drop down button
        rect.w += rt.h + rt.x;
    } else {
        //Add room for drop down button
        rt.w = rect.w - 3 * rt.x - rt.h;
    }
    rd.x = 2 * rt.x + rt.w;

    return rect;
}

Rect UIPainter::getComboOptionsRect(const Rect& rCombo, int numOptions, const Rect& options, Rect& ri, Rect& rit) const
{
    // the options frame is like a list box
    Rect rect = getListRect( Rect(), numOptions, options, ri, rit);

    // offset by the Combo box pos itself
    rect.x = rCombo.x;
    rect.y = rCombo.y - rect.h;

    return rect;
}

void UIPainter::drawComboBox(const Rect& r, int numOptions, const char *options[], const Rect& rt, const Rect& rd, int selected, bool isHover, bool isFocus, int style)
{
    drawFrame( r, Point(rt.x, rt.y), isHover, false, isFocus );
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), options[selected] );

    drawDownArrow( Rect(r.x + rd.x, r.y + rd.y, rd.w, rd.h), int(rd.h * 0.15f), 
        cBase + (!isHover) + (isFocus << 2), cOutline);
}

void UIPainter::drawComboOptions(const Rect& r, int numOptions, const char * options[], const Rect& ri, const Rect& rt, int selected, int hovered, bool isHover, bool isFocus, int style)
{
    drawListBox(r, numOptions, options, ri, rt, selected, hovered, style);
}


Rect UIPainter::getHorizontalSliderRect(const Rect& r, Rect& rs, float v, Rect& rc) const
{
    Rect rect = r;
    if (rect.w == 0)
        rect.w = getAutoWidth() + 2 * getWidgetMargin();
    if (rect.h == 0)
        rect.h = getAutoHeight() + 2 * getWidgetMargin();

    // Eval the sliding & cursor rect
    rs.y = getWidgetMargin();
    rs.h = rect.h - 2 * rs.y;
    rc.y = rs.y;
    rc.h = rs.h;

    rs.x = 0;//getWidgetMargin();
    rc.w = rc.h;
    rs.w = rect.w - 2 * rs.x - rc.w;
    rc.x = int(v * rs.w);

    return rect;
}

void UIPainter::drawHorizontalSlider(const Rect& r, Rect& rs, float v, Rect& rc, bool isHover, int style)
{
    int sliderHeight = rs.h / 3;
    drawFrame( Rect(r.x + rs.x, r.y + rs.y + sliderHeight, r.w - 2*rs.x, sliderHeight), 
        Point(sliderHeight / 2, sliderHeight / 2), isHover, false, false );
    drawFrame( Rect(r.x + rs.x + rc.x, r.y + rc.y, rc.w, rc.h), 
        Point(rc.w / 2, rc.h / 2), isHover, true, false );
}


void UIPainter::drawFrame( const Rect& rect, const Point& corner, bool isHover, bool isOn, bool isFocus)
 {
    int lColorNb = cBase + (isHover) + (isOn << 1);// + (isFocus << 2);

    if (corner.x + corner.y == 0)
        drawRect( rect , lColorNb, cOutline);
    else
        drawRoundedRect( rect, corner , lColorNb, cOutline );
}

void UIPainter::drawBoolFrame( const Rect& rect, const Point& corner, bool isHover, bool isOn, bool isFocus)
{
    int lColorNb = cBool + (isHover) + (isOn << 1);// + (isFocus << 2);

    drawRoundedRect( rect, corner , lColorNb, cOutline );
}

Rect UIPainter::getLineEditRect(const Rect& r, const Rect& text, Rect& rt) const
{
    Rect rect = r;
    rt.x = getWidgetMargin();
    rt.y = getWidgetMargin();

    if (rect.w == 0) {
        rt.w = std::max(text.w, getAutoWidth());
        rect.w = rt.w + 2 * rt.x;
    } else {
        rt.w = rect.w - 2 * rt.x;
    }
    if (rect.h == 0) {
        rt.h = text.h;
        rect.h = rt.h + 2 * rt.y;
    } else {
        rt.h = rect.h - 2 * rt.y;
    }
    
    return rect;
}

void UIPainter::drawLineEdit(const Rect& r, const char *text, const Rect& rt, int caretPos, bool isSelected, bool isHover, int style)
{
    drawFrame( r, Point( rt.x, rt.y ), true, isSelected, false );
    drawText( Rect(r.x + rt.x, r.y + rt.y, rt.w, rt.h), text, 1, caretPos);
}

void UIPainter::drawText( const Rect& r, const char *text, int nbLines, int caretPos, bool isHover, bool isOn, bool isFocus)
{
    if (isHover || isOn /* || isFocus*/) {
        drawRect(r, cFontBack + (isHover) + (isOn << 1), cOutline);
    }

    drawString(r.x, r.y, text, nbLines, cFont);

    if (caretPos != -1) {
        drawRect(Rect( r.x + caretPos, r.y, 2, r.h), cOutline, cOutline);
    }
}

}       // namespace

