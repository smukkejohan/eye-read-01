//
//  Paragraph.cpp
//  eye-read-01
//
//  Created by Johan Bichel Lindegaard on 26/03/2020.
//

#include "Paragraph.hpp"

Paragraph::Paragraph(std::string text, int width, Alignment align)
: mColor(ofColor::black)
, mIndent(40)
, mSpacing(6)
, mLeading(16)
, bDrawBorder(false)
, mBorderColor(ofColor::black)
, mBorderPadding(15)
, bDrawWordBoundaries(false)
, mWordBoundaryPadding(2)
, mWordBoundaryColor(ofColor::red)
{
    setText(text);
    setAlignment(align);
    setWidth(width);
};

void Paragraph::draw()
{
    ofPushStyle();
        for (int i=0; i<mWords.size(); i++) {
            ofSetColor(mColor);
            
            //ttf.drawStringAsShapes(mWords[i].text, this->x + mWords[i].rect.x, this->y + mWords[i].rect.y);
            //ttf.getStringMesh(mWords[i].text, this->x + mWords[i].rect.x, this->y + mWords[i].rect.y).draw();
            //ttf.getStringTexture(mWords[i].text).draw(this->x + mWords[i].rect.x, this->y + mWords[i].rect.y, false);
            
            ttf.drawString(mWords[i].text, this->x + mWords[i].rect.x, this->y + mWords[i].rect.y);
            
            if (bDrawWordBoundaries == true){
                ofPushStyle();
                ofNoFill();
                ofSetColor(mWordBoundaryColor);
                ofDrawRectangle(this->x + mWords[i].rect.x - mWordBoundaryPadding,
                       this->y + mWords[i].rect.y-mLineHeight - mWordBoundaryPadding,
                       mWords[i].rect.width + (mWordBoundaryPadding * 2),
                       mLineHeight + (mWordBoundaryPadding * 2));
                ofPopStyle();
            }
        }
        if (bDrawBorder == true){
            ofNoFill();
            ofSetColor(mBorderColor);
            ofDrawRectangle(this->x - mBorderPadding,
                   this->y - ttf.getLineHeight() - mBorderPadding,
                   mWidth + (mBorderPadding * 2),
                   mHeight + (mBorderPadding * 2));
        }
    ofPopStyle();
}

void Paragraph::draw(int x, int y)
{
    this->x = x;
    this->y = y;
    draw();
}

void Paragraph::setColor(ofColor color)
{
    mColor = color;
}

void Paragraph::setColor(int color)
{
    mColor = ofColor::fromHex(color);
}

int Paragraph::getWidth()
{
    return mWidth;
}

int Paragraph::getHeight()
{
    return mHeight;
}

int Paragraph::getStringHeight(std::string s)
{
    if (s == "") s = "ABCDEFGHIJKLMNOPQWXYZ1234567890";
    return ttf.getStringBoundingBox(s,0,0).height;
}

void Paragraph::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Paragraph::drawBorder(bool draw)
{
    bDrawBorder = draw;
}

void Paragraph::drawBorder(ofColor color)
{
    bDrawBorder = true;
    mBorderColor = color;
}

void Paragraph::setBorderPadding(int padding)
{
    mBorderPadding = padding;
}

void Paragraph::drawWordBoundaries(bool draw)
{
    bDrawWordBoundaries = draw;
}

/*
    re-rendering methods
*/

void Paragraph::setText(std::string text)
{
    mText = trim(text);
    render();
}

void Paragraph::setWidth(int width)
{
    mWidth = width;
    render();
}

void Paragraph::setSpacing(int spacing)
{
    mSpacing = spacing;
    render();
}

void Paragraph::setLeading(int leading)
{
    mLeading = leading;
    render();
}

void Paragraph::setIndent(int indent)
{
    mIndent = indent;
    render();
}

void Paragraph::setAlignment(Alignment align)
{
    mAlignment = align;
    render();
}

/*void Paragraph::setFont(std::shared_ptr<ofxSmartFont> ttf)
{
    mFont = ttf;
    render();
}*/

void Paragraph::setFont(std::string file, int size)
{
    //mFont = ofxSmartFont::add(file, size, name);
    ttf.load(file, size, true, true, true, 0.3f, 0);
    render();
}

void Paragraph::render()
{
    //if (mFont == nullptr) return;
    mWords.clear();
    mLineHeight = 0;
    std::string str = mText;
    
// break paragraph into words //
    int position = str.find(" ");
    while ( position != std::string::npos )
    {
        std::string s = str.substr(0, position);
        word w = {s, ttf.getStringBoundingBox(s,0,0)};
        mWords.push_back(w);
        str.erase(0, position + 1);
        position = str.find(" ");
        if (w.rect.height > mLineHeight) mLineHeight = w.rect.height;
    }
// append the last word //
    word w = {str, ttf.getStringBoundingBox(str,0,0)};
    mWords.push_back(w);
    if (w.rect.height > mLineHeight) mLineHeight = w.rect.height;
    
// assign words to lines //
    int y = 0;
    int x = mAlignment == ALIGN_LEFT ? mIndent : 0;
    mLines.clear();
    std::vector<word*> line;
    for (int i=0; i<mWords.size(); i++) {
        if (x + mWords[i].rect.width < mWidth){
            mWords[i].rect.x = x;
            mWords[i].rect.y = y;
            x += mWords[i].rect.width + mSpacing;
            line.push_back(&mWords[i]);
        }   else{
            if (line.size() > 0 ) y+= mLineHeight + mLeading;
            mWords[i].rect.x = 0;
            mWords[i].rect.y = y;
            x = mWords[i].rect.width + mSpacing;
            if (line.size() > 0) mLines.push_back(line);
            line.clear();
            line.push_back(&mWords[i]);
        }
    }
// append the last line //
    mLines.push_back(line);
    mHeight = mLines.size() * (mLineHeight + mLeading);
    
// reposition words for right & center aligned paragraphs //
    if (mAlignment == ALIGN_CENTER){
        for(int i=0; i<mLines.size(); i++) {
            int lineWidth = 0;
            for(int j=0; j<mLines[i].size(); j++) {
                lineWidth+= mLines[i][j]->rect.width;
            }
            lineWidth+= mSpacing * (mLines[i].size()-1);
        // calculate the amount each word should move over //
            int offset = (mWidth - lineWidth) / 2;
            for(int j=0; j<mLines[i].size(); j++) mLines[i][j]->rect.x += offset;
        }
    }   else if (mAlignment == ALIGN_RIGHT){
        for(int i=0; i<mLines.size(); i++) {
            word* lword = mLines[i].back();
    // calculate the distance the last word in each line is from the right boundary //
            int offset = mWidth - (lword->rect.x + lword->rect.width);
            for(int j=0; j<mLines[i].size(); j++) mLines[i][j]->rect.x += offset;
        }
    }
}

