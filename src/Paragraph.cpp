//
//  Paragraph.cpp
//  eye-read-01
//
//  Created by Johan Bichel Lindegaard on 26/03/2020.
//

#include "Paragraph.hpp"

Paragraph::Paragraph(std::string text, int width, Alignment align)
: mColor(ofColor::black)
, mIndent(0)
, mSpacing(6)
, mLeading(16)
, bDrawBorder(false)
, mBorderColor(ofColor::black)
, mBorderPadding(15*10)
, bDrawWordBoundaries(false)
, mWordBoundaryPadding(2)
, mWordBoundaryColor(ofColor::red)
, magnifyScale(4)
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
    
    ofTrueTypeFontSettings settings(file, size);
    settings.addRange(ofUnicode::Latin);
    settings.addRange(ofUnicode::Latin1Supplement);
    settings.antialiased = true;
    settings.contours = true;
    settings.simplifyAmt = 0.01f;
    settings.dpi = 96;
    
    ttf.load(settings);
    
    ofTrueTypeFontSettings bigSettings(file, size*magnifyScale);
    bigSettings.addRange(ofUnicode::Latin);
    bigSettings.addRange(ofUnicode::Latin1Supplement);
    bigSettings.antialiased = true;
    bigSettings.contours = true;
    bigSettings.simplifyAmt = 0.01f;
    bigSettings.dpi = 96*DPI_SCALE_FACTOR;
    
    ttfBig.load(bigSettings);
    render();
}

void Paragraph::render()
{
    
    //if (mFont == nullptr) return;
    if(!ttf.isLoaded()) {
        return;
    }
    
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
    
    mLetters.clear();
    
    float lineY = 0; // mLeading;

    for(auto &line : mLines) {
        
        for(auto &w : line) {
            // for word in line
            //std::vector<char> letters(w->text.begin(), w->text.end());
            
            // Our letter position does not align exactly.
            // look at: https://github.com/sosolimited/ofxSoso/blob/master/src/ofxSosoTrueTypeFont.cpp#L1049
            // we also do not handle ligatures at the moment
            
            std::vector<ofPath> letterPaths =  ttfBig.getStringAsPoints(w->text);
            w->letterPaths = letterPaths;
            
            float lX = w->rect.getX();
            
            //std::cout<<"Letters: ";
            for(int i=0; i <= ofUTF8Length(w->text); i++) {
                
                 std::string s = ofUTF8Substring(w->text, i,1);
                 //std::string s(1, c);
                
                 ofRectangle b = ttf.getStringBoundingBox(s, lX, lineY);
                 lX += b.getWidth() + ttf.getLetterSpacing();
                 //b.setY(lineY - mLineHeight);
                
                 letter newLetter = {s, b};
                 mLetters.push_back(newLetter);
                 w->letters.push_back(newLetter);
                 
                 //mLetters.push new letter
             }
            
            /*for(auto &path : letterPaths) {
            
                //ofRectangle b = ttf.getStringBoundingBox(s, lX, 0);
                //lX += b.getWidth() + ttf.getLetterSpacing();
                //b.setY(lineY);
                // add path to our word struct instead ?
                letterPath newPath = {path};
                mLetterPaths.push_back(newPath);
                //mLetters.push new letter
            }*/
            //std::cout<<"|END"<<std::endl;
        }
        
        lineY += (mLineHeight + mLeading);
    }
    
    spaceWidthPx = ttf.getStringBoundingBox("a", 0, 0).width * 0.25;

}


void Paragraph::drawLetterAtPos( float x, float y) {
}


// a boolean function that tells ofSort how to compare two items
bool sortWordsByDistance(Paragraph::word & a, Paragraph::word & b){
    if (a._yDist == b._yDist){
        // same line
        if (a._xDist > b._xDist){
            return true;
        } else {
            return false;
        }
    } else {
        if (a._yDist > b._yDist){
            return true;
        } else {
            return false;
        }
    }
}


void Paragraph::calculateAttractPoint(float x, float y) {
    
    ofVec2f pos(x - this->x, y - this->y);
    
    // get closest line // NOT necesarily in it TODO
    // use line to attract Y position
    for(auto &line : mLines) {
        
        ofRectangle lineRect(0,
        line.front()->rect.y - mLineHeight - mWordBoundaryPadding,
        mWidth, mLineHeight + (mWordBoundaryPadding * 2));
        // set rect of last to end at bottom and rect of first to end at top
           
        if(lineRect.inside(pos)) {
            // save the current line
            
            attractPoint.y = this->y + line.front()->rect.y - mWordBoundaryPadding; //- (mLineHeight + mWordBoundaryPadding)*0.5;
            
            // get closest word / letter in line - use centroid.x to attract x position
            // x axis effect should be less than y axis
            for (std::size_t i = 0, e = line.size(); i != e; ++i) {
                auto &w = line.at(i);
                // Get the closest word
                if( pos.x < w->rect.getX() + w->rect.width ) {
                    // attractPoint.x = this->x + w->rect.getX() + w->rect.width*0.5; // snap to center of word
                    attractPoint.x = pos.x + this->x;
                   break;
                }
            }
            break;
        }
    }
    
}

void Paragraph::calculateAttractPointScrolling(float x, float y) {
    
    if(freezeLastWord ) {
        ofVec2f p = nextLine.front()->rect.position;
        
        // TODO: don't continue if we have not moved down the page
        attractPoint = ofVec2f(this->x + p.x, this->y + p.y);
        
        return;
    }

    
    ofVec2f pos(x - this->x, y - this->y);
    
    // get closest line // NOT necesarily in it TODO
    // use line to attract Y position
    attractPoint.x = pos.x + this->x;
    

    for(auto &line : mLines) {
        
        ofRectangle lineRect(0,
        line.front()->rect.y - mLineHeight - mWordBoundaryPadding,
        mWidth, mLineHeight + (mWordBoundaryPadding * 2));
        // set rect of last to end at bottom and rect of first to end at top
           
        if(lineRect.inside(pos)) {
            // save the current line
            attractPoint.y = this->y + line.front()->rect.y - mWordBoundaryPadding; //- (mLineHeight + mWordBoundaryPadding)*0.5;
            
            break;
        }
    }
    
}


void Paragraph::drawScrollingLine() {
    const double scale = (1.0/DPI_SCALE_FACTOR) * (1.0/magnifyScale) * magnifyScale;

    ofPushMatrix();
    ofTranslate(this->x, this->y);
    ofPushStyle();
    // Draw other lines
    for(auto &line : mLines) {
        if(line != currentLine && line != nextLine) {
            ofSetColor(80,80,80);
            for(auto &w : line) {
                ttf.drawString(w->text, w->rect.x, w->rect.y);
            }
        }
    }
    
    if(currentLine.size() > 0) {
        // Draw current line magnified - using eye movement to scroll
        
            std::string maglinetxt = "";
            
            if(isLastWord) {
                
                /*for(auto &w : currentLine) {
                    if(w != currentWord) {
                        ttf.drawString(w->text, w->rect.x, w->rect.y);
                    }
                }*/
                std::string linetxt = "";
                

                for(int i=0; i < currentLine.size()-1; i++ ) {
                    auto &w = currentLine.at(i);
                    linetxt += w->text;
                    linetxt += " ";
                }
                

                maglinetxt += currentLine.back()->text;
                    
                ofPushMatrix();
                ofTranslate(currentLine.front()->rect.x, currentLine.front()->rect.y);
                ofScale(scale, scale);
                        
                // draw background
                ofRectangle bounding = ttfBig.getStringBoundingBox(maglinetxt, 0, 0);
                    
                int rJ = (mWidth/scale) - bounding.width;
                bounding.setX(rJ);
                    
                if(ofGetElapsedTimeMillis() - freezeLastWordTime < freezeLastWordDwellTime) {
                    
                    ofSetColor(255,255,255);
                    ofDrawRectangle(bounding);
                    ofSetColor(0,0,0);
                    ttfBig.drawStringAsShapes(maglinetxt, rJ,  0);
                }
                    
                ofPopMatrix();
                    
                ofRectangle prl = ttf.getStringBoundingBox(linetxt, 0, 0);
                float wD = mWidth - prl.width;
                    
                if(ofGetElapsedTimeMillis() - freezeLastWordTime > freezeLastWordDwellTime) {
                    linetxt += currentLine.back()->text;
                }
                
                ofSetColor(0,0,0);
                ttf.drawString(linetxt, currentLine.front()->rect.x - (bounding.width*scale - wD), currentLine.front()->rect.y);

            } else {
                
                for(auto &w : currentLine) {
                    maglinetxt += w->text;
                    if(w != currentLine.back()) {
                        maglinetxt += " ";
                    }
                }
                
                ofPushMatrix();
                ofTranslate(currentLine.front()->rect.x, currentLine.front()->rect.y);
                ofScale(scale, scale);
                    
                // draw background
                ofSetColor(255,255,255);
                ofRectangle bounding = ttfBig.getStringBoundingBox(maglinetxt, 0, 0);
                ofDrawRectangle(bounding);
            
                ofSetColor(0,0,0);
                ttfBig.drawStringAsShapes(maglinetxt, (focusPos.x/scale) + (-scrollIn * bounding.width),  0);
                
                ofPopMatrix();
                
            }
        
        // Mask off magnified outside of paragraph
        ofSetColor(255,255,255);
        ofDrawRectangle(-this->x, currentLine.front()->rect.y - mLineHeight * magnifyScale, this->x, mLineHeight * (magnifyScale+1));
        ofDrawRectangle(mWidth, currentLine.front()->rect.y - mLineHeight * magnifyScale, this->x, mLineHeight * (magnifyScale+1));
        
    }

        
    if(isLastWord && ofGetElapsedTimeMillis() - freezeLastWordTime > freezeLastWordDwellTime) {
        
        if(nextLine.size() > 0) {
            
            std::string linetxt = " ";

            for(int i=1; i < nextLine.size(); i++ ) {
                auto &w = nextLine.at(i);
                linetxt += w->text;
                
                linetxt += " ";
            }
            
            // Draw next line magnified
            
                std::string maglinetxt = "";
            
                maglinetxt += nextLine.front()->text;
    
                ofPushMatrix();
                ofTranslate(nextLine.front()->rect.x, nextLine.front()->rect.y);
                ofScale(scale, scale);

                //ofScale(0.025,0.025);
                    
                // draw background
                ofSetColor(255,255,255);
                ofRectangle bounding = ttfBig.getStringBoundingBox(maglinetxt, 0, 0);
            ofDrawRectangle(0, bounding.y, bounding.width+4, bounding.height+2 );
                    
                ofSetColor(0,0,0);
                ttfBig.drawStringAsShapes(maglinetxt, 0, 0);
            
                ofPopMatrix();
            
            ofRectangle prl = ttf.getStringBoundingBox(linetxt, 0, 0);

            float wD = currentLineWidth - prl.width;
            
            ttf.drawString(linetxt, bounding.width*scale, nextLine.front()->rect.y);
            
            // Mask off magnified outside of paragraph
            ofSetColor(255,255,255);
            ofDrawRectangle(-this->x, nextLine.front()->rect.y - mLineHeight * magnifyScale, this->x, mLineHeight * (magnifyScale+1));
            ofDrawRectangle(mWidth, nextLine.front()->rect.y - mLineHeight * magnifyScale, this->x, mLineHeight * (magnifyScale+1));
            
        }
        
    } else if(nextLine.size() > 0) {
        
        for(auto &w : nextLine) {
            ofSetColor(0,0,0);
            ttf.drawString(w->text, w->rect.x, w->rect.y);
        }
        
    }
    
    ofPopStyle();
    
    //ofDrawCircle(focusPos, 20);
    
    ofPopMatrix();
    
    // ofDrawCircle(draw_magnifyBounding.width/2 , linePos, 20);
    
    ofSetColor(0,0,0);
    
    ofDrawBitmapString(currentLineNumber, 40, ofGetHeight() - 80);
    ofDrawBitmapString(isLastWord, 40, ofGetHeight() - 60);
    ofDrawBitmapString(ofGetElapsedTimeMillis() - freezeLastWordTime, 40, ofGetHeight() - 40);
    
    
    //ofSetColor(0,0,0, 200);

    
}

void Paragraph::calculateScrollingLine(float x, float y, float rawx, float rawy) {
    
    ofVec2f nP = ofVec2f(rawx - this->x, rawy - this->y);
    const double scale = (1.0/DPI_SCALE_FACTOR) * (1.0/magnifyScale) * magnifyScale;

    isLastWord = false;
    bool inLastWord = false;
    
    if(freezeLastWord) {
        
        ofRectangle bounding = ttf.getStringBoundingBox(nextLine.front()->text, 0, 0);
        
        if(nextLineTargetReached) {
            
            if(ofVec2f(nextLine.front()->rect.position).distance(nP) > bounding.width*scale ) {
                // target reached and exited, start new dynamic magnification
                std::cout<<"target reached and exited"<<std::endl;
                nextLineTargetReached = false;
                freezeLastWord = false;
                focusPos = ofVec2f(x - this->x, y - this->y);
            }
            
        } else if( ofVec2f(nextLine.front()->rect.position).distance(nP) < bounding.width*scale ) {
            nextLineTargetReached = true;
        }
    } else {
        focusPos = ofVec2f(x - this->x, y - this->y);
    }
    
    int ln = 0;
    currentLineWidth = 0;
    
    for(auto &line : mLines) {
        ln++;
        
        ofRectangle lineRect(0, line.front()->rect.y - mLineHeight - mWordBoundaryPadding,
                             mWidth+200, mLineHeight + (mWordBoundaryPadding * 2));
        
           
        if( lineRect.inside(focusPos) ) {
            currentLine = line;
            
            if( mLines.size() > ln ) {
                nextLine = mLines.at(ln);
            } else {
                nextLine.clear();
            }
            currentLineNumber = ln;
            
            inLastWord = lineRect.inside(nP) && nP.x >= line.back()->rect.getMinX();
            
            for (std::size_t i = 0, eL = currentLine.size(); i != eL; ++i) {
                auto &w = currentLine.at(i);
                   
                ofRectangle r(w->rect.x - (mWordBoundaryPadding + spaceWidthPx),
                       w->rect.y - mLineHeight*2.25 - mWordBoundaryPadding,
                       w->rect.width + (mWordBoundaryPadding + spaceWidthPx)*2,
                       mLineHeight*4 + (mWordBoundaryPadding * 2));
                
                currentLineWidth += r.width;
                
                if( r.inside(focusPos) ) {
                    currentWord = w;
                }
                
                if(i == currentLine.size()-1 ) {
                    if(focusPos.x > r.getMinX()) {
                        if(nextLine.size() > 0) {
                            isLastWord = true;
                        }
                    }
                }
            }
        }
    }
    

    if(isLastWord && !freezeLastWord /*&& ofGetElapsedTimeMillis() - freezeLastWordTime > lineTransitionDwellTime*/  ) {
        freezeLastWordTime = ofGetElapsedTimeMillis();
        freezeLastWord = true;
        
    } else {
    }
    
    //std::cout<<inLastWord<<std::endl;
    
    if ( ofGetElapsedTimeMillis() - freezeLastWordTime > lineTransitionDwellTime) {
        
        if(!inLastWord) {
            freezeLastWord = false;
        }
        
    }

    focusPos.x = ofClamp(focusPos.x, 0, currentLineWidth);
    scrollIn = ofMap(focusPos.x, 0, currentLineWidth, 0, 1);
}



void Paragraph::calculateMagnifiedLetters(float x, float y, int numLettersLeft, int numLettersRight, bool push, bool magnifyWholeWords) {
        
    focusPos = ofVec2f(x - this->x, y - this->y);
    
    for(auto &line : mLines) {
        ofRectangle lineRect(0,
        line.front()->rect.y - mLineHeight - mWordBoundaryPadding,
        mWidth, mLineHeight + (mWordBoundaryPadding * 2));
           
        if(!lineRect.inside(focusPos)) {
            for(auto &w : line) {
                   ofSetColor(80,80,80);
                   ttf.drawString(w->text, w->rect.x, w->rect.y);
            }
        } else {
            // save the line we need to magnify
            currentLine = line;
        }
    }
            
    next_leftLettersStr = "";
    next_rightLettersStr = "";
    next_wordsBefore.clear();
    next_wordsAfter.clear();

    bool before = true;
       
    for (std::size_t i = 0, eL = currentLine.size(); i != eL; ++i) {
        auto &w = currentLine.at(i);
           
        ofRectangle r(w->rect.x - (mWordBoundaryPadding + spaceWidthPx),
               w->rect.y - mLineHeight*2.25 - mWordBoundaryPadding,
               w->rect.width + (mWordBoundaryPadding + spaceWidthPx)*2,
               mLineHeight*4 + (mWordBoundaryPadding * 2));
           
        if( r.inside(focusPos)) {
            currentWord = w;
               
            bool currentLetterFound = false;
               
            for (std::size_t ii = 0, e = w->letters.size(); ii != e; ++ii) {
                auto l = w->letters.at(ii);
                   
                   if( (!currentLetterFound && ( focusPos.x >= l.rect.getLeft()-spaceWidthPx || ii==0 ) && (focusPos.x <= l.rect.getRight()+spaceWidthPx || ii==w->letters.size() ))
                      ) {
                       
                       currentLetterFound = true;
                       
                       next_magnifyStr = l.text;
                       next_magnifyPos.y = w->rect.y + mWordBoundaryPadding ;
                       
                       int letterCount = 0;
                       int wordIndex = i;
                       int letterIndex = ii;
                       
                       bool drawLeft = true;
                       
                       while(drawLeft) {
                           
                           if(letterCount >= numLettersLeft) {
                               if(magnifyWholeWords) {
                                   if(letterIndex == 0) {
                                       drawLeft = false;
                                   }
                               } else {
                                   drawLeft = false;
                               }
                           }
                           
                           // Add letters before
                           // Get in words, break if line ends
                           letterIndex--;
                           if( letterIndex >= 0) {
                               next_magnifyStr = currentLine.at(wordIndex)->letters.at(letterIndex).text + next_magnifyStr;
                               letterCount++;
                               
                           } else {
                               
                               // get word before
                               wordIndex--;
                               if(wordIndex < 0) {
                                   break;
                               } else {
                                   
                                   letterIndex = currentLine.at(wordIndex)->letters.size();
                                   next_magnifyStr = " " + next_magnifyStr;
                               }
                           }
                       }
                       
                       if(wordIndex >= 0) {
                           letterIndex--;
                           if(currentLine.at(wordIndex)->letters.size() - letterIndex > 0 ) {
                                 for (int _i = letterIndex; _i >= 0; --_i) {
                                     next_leftLettersStr = currentLine.at(wordIndex)->letters.at(_i).text + next_leftLettersStr;
                                 }
                               
                                 next_leftWordX = currentLine.at(wordIndex)->rect.x;
                                 wordIndex--;
                             }
                           
                             for (int _i = wordIndex; _i >= 0; --_i) {
                                 next_wordsBefore.push_back( currentLine.at(_i) );
                             }
                       }
     
                       next_xOffsetLeft = ttfBig.getStringBoundingBox(next_magnifyStr, 0, 0).width / 10;
                       next_magnifyPos.x = l.rect.getPosition().x - next_xOffsetLeft;
                       
                       letterCount = 0;
                       wordIndex = i;
                       letterIndex = ii + 1;
                       
                       bool drawRight = true;
                       while(drawRight) {
                           // Add letters after
                           // Get in words, break if line ends
                           if(letterCount >= numLettersRight) {
                               if(magnifyWholeWords) {
                                   if(letterIndex == currentLine.at(wordIndex)->letters.size() ) {
                                       drawRight = false;
                                   }
                               } else {
                                   drawRight = false;
                               }
                           }
                           
                           if( letterIndex < currentLine.at(wordIndex)->letters.size() ) {
                               next_magnifyStr = next_magnifyStr + currentLine.at(wordIndex)->letters.at(letterIndex).text;
                               letterIndex++;
                               letterCount++;
                               
                           } else {
                               // get word after
                               wordIndex++;
                               if(wordIndex >= currentLine.size()) {
                                   break;
                               } else {
                                   letterIndex = 0;
                                   next_magnifyStr = next_magnifyStr + " ";
                               }
                           }
                       }
                       
                       if(wordIndex < currentLine.size()) {
                           
                           if( letterIndex <= currentLine.at(wordIndex)->letters.size() ) {
                                                         
                                 for (int _i = letterIndex; _i < currentLine.at(wordIndex)->letters.size(); ++_i) {
                                     next_rightLettersStr = next_rightLettersStr + currentLine.at(wordIndex)->letters.at(_i).text;
                                 }
                               
                                 next_rightWordX = currentLine.at(wordIndex)->rect.x + (currentLine.at(wordIndex)->rect.width - ttf.getStringBoundingBox(next_rightLettersStr, 0, 0).width);
                                 wordIndex++;
                             }
                           
                           for (int _i = wordIndex; _i < currentLine.size(); ++_i) {
                               next_wordsAfter.push_back( currentLine.at(_i) );
                           }
                       }
                       
                       before = false;
                       
                   }
               }
           }
       }
}


void Paragraph::drawMagnifiedLetters(float x, float y, bool push, bool magnifyWholeWords) {
    // draw original in red to debug alignment
    
    if(draw_magnifyStr != next_magnifyStr) {
        
        draw_magnifyStr = next_magnifyStr;
        
        draw_leftLettersStr = next_leftLettersStr;
        draw_rightLettersStr = next_rightLettersStr;
        
        draw_wordsBefore = next_wordsBefore;
        draw_wordsAfter = next_wordsAfter;
           
        draw_leftWordX = ofLerp(draw_leftWordX, next_leftWordX, 1);
        draw_rightWordX = ofLerp(draw_rightWordX, next_rightWordX, 1);
        draw_xOffsetLeft = ofLerp(draw_xOffsetLeft, next_xOffsetLeft, 1);
        
        draw_magnifyPos = draw_magnifyPos.getInterpolated(next_magnifyPos, 1);
        
        next_magnifyBounding = ttfBig.getStringBoundingBox(next_magnifyStr, 0, 0);
        next_xOffsetRight = (next_magnifyBounding.width /10) - next_xOffsetLeft;

        draw_xOffsetRight = ofLerp(draw_xOffsetRight, next_xOffsetRight, 1);
        
        draw_magnifyBounding = next_magnifyBounding;
        
    }

    
    ofPushMatrix();
    ofTranslate(this->x, this->y);
    ofPushStyle();
    
    for(auto &line : mLines) {
        if(line != currentLine) {
            for(auto &w : line) {
                   ofSetColor(80,80,80);
                   ttf.drawString(w->text, w->rect.x, w->rect.y);
            }
        }
    }
    
    // Then take care of the magnified line word by word path by path
    float bigWordPadding = mWordBoundaryPadding * magnifyScale;
    
    if(!push) {
        draw_xOffsetRight = 0;
        draw_xOffsetLeft = 0;
    }
    
    if(currentLine.size() > 0 ) {
        
        int linePos = currentLine.front()->rect.y;
           
           ofSetColor(30,30,30);

           for(auto &w : draw_wordsBefore) {
               ttf.drawString(w->text, w->rect.x - draw_xOffsetLeft, linePos);
           }
           
           if(draw_leftLettersStr.size() > 0) {
               ttf.drawString(draw_leftLettersStr, draw_leftWordX - draw_xOffsetLeft, linePos);
           }
           
           for(auto &w : draw_wordsAfter) {
               ttf.drawString(w->text, w->rect.x + draw_xOffsetRight, linePos);
           }
           
           if(draw_rightLettersStr.size() > 0) {
               ttf.drawString(draw_rightLettersStr, draw_rightWordX + draw_xOffsetRight, linePos);
           }
           
           // draw white bacground behind magnified
        
        ofPushMatrix();

            ofTranslate(draw_magnifyPos.x, linePos);
            ofScale(0.1,0.1);

           ofSetColor(255,255,255);
           ofDrawRectangle(draw_magnifyBounding);
           
           ofSetColor(0,0,0);
           ttfBig.drawStringAsShapes(draw_magnifyStr, 0, 0);
        
       // ofDrawCircle(draw_magnifyBounding.width/2 , linePos, 20);
        
        
        
        
        ofPopMatrix();
        
           /*ttfBig.drawString(draw_magnifyStr, draw_magnifyPos.x, draw_magnifyPos.y);*/
           //}
           
           /*ofPushStyle();
           for(auto &l : mLetters) {
               ofSetColor(0,0,200);
               ttf.drawString(l.text , this->x + l.rect.x, this->y + l.rect.y );
           }
           ofPopStyle();*/
           
           /*ofPushStyle();
           ofSetColor(0,0,0);
           for(auto &path : mLetterPaths) {
               path.path.draw(this->x, this->y);
           }
           ofPopStyle();*/
           
    }
    
    ofPopStyle();
    ofPopMatrix();
}


// draw segments
// 




void Paragraph::drawMagnified1(float x, float y, float scale) { // Scale is 1 to bigFontMult
    
    float maxDist = 400;
    float bigWordPadding = mWordBoundaryPadding * magnifyScale;
    //ofVec2f focusPos(x - this->x, y - this->y);
    
    for(auto &w : mWords) {
        ofRectangle r(w.rect.x - mWordBoundaryPadding,
        w.rect.y - mLineHeight - mWordBoundaryPadding,
        w.rect.width + (mWordBoundaryPadding * 2),
                      mLineHeight + (mWordBoundaryPadding * 2));
        
        ofVec2f wordCenter(r.x + (r.width / 2), (r.y + (r.height / 2)));
        
        w._xDist = abs(focusPos.x - wordCenter.x);
        w._yDist = abs(focusPos.y - wordCenter.y);
        w.pushOut = 0;
    }
    
    ofSort(mWords, sortWordsByDistance);
    
    float pushBack = 0;
    float pushFront = 0;
    
    for( auto it = mWords.rbegin(); it != mWords.rend(); ++it ) {
            //*it.pushOut =
        word & w = *it;
        w.pushOut = 0;
        //std::cout<<" it " << w.text << std::endl;
        
        ofRectangle r(w.rect.x - mWordBoundaryPadding,
        w.rect.y - mLineHeight - mWordBoundaryPadding,
        w.rect.width + (mWordBoundaryPadding * 2),
                      mLineHeight + (mWordBoundaryPadding * 2));
        
        ofVec2f wordCenter(r.x + (r.width / 2), (r.y + (r.height / 2)));
        
        if( w._xDist < maxDist && r.inside(wordCenter.x,  focusPos.y)) { // multiple words on the line
            float _s = ofMap(w._xDist, 0, r.width*2, 1, scale, true);
            
            ofRectangle bigBounding = ttfBig.getStringBoundingBox(w.text, r.x, w.rect.y);
            ofRectangle bigR(r.x + r.width/2 - (bigBounding.width/2 + bigWordPadding), w.rect.y - (bigBounding.height + bigWordPadding), bigBounding.width + bigWordPadding*2, bigBounding.height + bigWordPadding*2);
            
            if(pushFront == 0) { // first
                pushBack -= (bigR.width/2)* 1-(1/_s);
                pushFront += (bigR.width/2)* 1-(1/_s);
            } else {
                if(wordCenter.x < focusPos.x) {
                    w.pushOut = pushBack;//ofLerp(w.pushOut, pushBack, 0.5);
                    pushBack -= (bigR.width)* 1-(1/_s);

                } else {
                    w.pushOut = pushFront;//ofLerp(w.pushOut, pushFront, 0.5);
                    pushFront += (bigR.width)* 1-(1/_s);
                }
            }
        }
    }

        
    for(auto w : mWords) {
        
        ofRectangle r(w.rect.x - mWordBoundaryPadding,
        w.rect.y - mLineHeight - mWordBoundaryPadding,
        w.rect.width + (mWordBoundaryPadding * 2),
                      mLineHeight + (mWordBoundaryPadding * 2));
        
        ofVec2f wordCenter(r.x + (r.width / 2), (r.y + (r.height / 2)));
        // Current word
                
        //float yDist = abs((y - this->y) - wordCenter.y);
        //float xDist = abs((x - this->x) - wordCenter.x);
        //float dist = focusPos.distance(ofVec2f(r.x + (r.width / 2), r.y + (r.height / 2)) );
        
        if( w._xDist < maxDist && r.inside(wordCenter.x,  focusPos.y)) { // multiple words on the line
        
        //if(true) {
            float _s = ofMap(w._xDist, 0, r.width*2, 1, scale, true);

        //if(r.inside(focusPos)) { // one word
            //float _s = ofMap(dist, 0, r.width/2, scale, 1, true);
            
            //std::cout<<w._xDist<<" "<<scale<<std::endl;
            
            ofPushStyle();
            ofPushMatrix();

            ofTranslate(this->x, this->y);
            
            //ofPushMatrix();
            
            /*ofTranslate(r.x + r.width/2, w.rect.y);
            ofScale(scale,scale);
            ofTranslate(-(r.x + r.width/2), -w.rect.y);*/
            
            /*
             ofPushStyle();
            ofNoFill();
            ofSetColor(255,0,0);
            ofDrawRectangle(r);
            ofPopStyle();
            */
            
            //ofDrawRectangle(w.rect);
            //ofPopMatrix();
            
            ofSetColor(0, 0, 255);
            
            ofPushMatrix();
            ofTranslate(r.x + r.width/2, w.rect.y);
            ofScale((1/_s), (1/_s) );
            ofTranslate(-(r.x + r.width/2), -w.rect.y);
            
            //ofTranslate(w.pushOut, 0);
                                    
            ofRectangle bigBounding = ttfBig.getStringBoundingBox(w.text, r.x, w.rect.y);
            ofRectangle bigR(r.x + r.width/2 - (bigBounding.width/2 + bigWordPadding), w.rect.y - (bigBounding.height + bigWordPadding), bigBounding.width + bigWordPadding*2, bigBounding.height + bigWordPadding*2);
            
            ofSetColor(255,255,255);
            ofDrawRectangle(bigR);
            
            ofSetColor(0);
            //ofDrawCircle(r.x+ r.width/2,r.y,4); // Draw word center

            ttfBig.drawString(w.text, r.x + r.width/2 - bigBounding.width/2, w.rect.y);
            ofPopMatrix();
                        
            ofPopMatrix();
            ofPopStyle();
            
            //return;

        } else {
            
            ofSetColor(0);
            ttf.drawString(w.text, this->x + w.rect.x, this->y + w.rect.y);

        }
        
    }
}


void Paragraph::drawNearestWord(float x, float y) {
    
    for(auto w : mWords) {
        if(w.rect.inside( x- this->x ,  y-this->y)) {
            ttf.drawString(w.text, this->x + w.rect.x, this->y + w.rect.y);
            return;
        }
    }
}


void Paragraph::getLetterCentroid(float x, float y) {
    
}




