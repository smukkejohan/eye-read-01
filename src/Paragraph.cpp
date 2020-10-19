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
, bigFontMult(4)
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
    // increase dpi to get smooth shapes ?
    ttf.load(file, size, true, true, true, 0.3f, 96); // default 96
    ttfBig.load(file, size*bigFontMult, true, true, true, 0.3f, 96/**bigFontMult*/); // poor anti aliasing of scaled down ig text ??
    
    // Set spacing when scaling up 

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

// dwell  calculation ... ish
void Paragraph::calculateAttractPoint(float x, float y) {
    
    ofVec2f focusPos(x - this->x, y - this->y);
    
    // get closest line // NOT necesarily in it TODO
    // use line to attract Y position
    for(auto &line : mLines) {
        
        ofRectangle lineRect(0,
        line.front()->rect.y - mLineHeight - mWordBoundaryPadding,
        mWidth, mLineHeight + (mWordBoundaryPadding * 2));
        // set rect of last to end at bottom and rect of first to end at top
           
        if(lineRect.inside(focusPos)) {
            // save the current line
            attractPoint.y = this->y + line.front()->rect.y - mWordBoundaryPadding; //- (mLineHeight + mWordBoundaryPadding)*0.5;
            
            // get closest word / letter in line - use centroid.x to attract x position
            // x axis effect should be less than y axis
            for (std::size_t i = 0, e = line.size(); i != e; ++i) {
                auto &w = line.at(i);
                // Get the closest word
                if( focusPos.x < w->rect.getX() + w->rect.width ) {
                    // attractPoint.x = this->x + w->rect.getX() + w->rect.width*0.5; // snap to center of word
                    attractPoint.x = focusPos.x + this->x;
                   break;
                }
            }
            break;
        }
    }
    
    

    
}

void Paragraph::drawMagnifiedLetters(float x, float y, int numLettersLeft, int numLettersRight, bool push, float scale, bool magnifyWholeWords) { // Scale is 1 to bigFontMult
    // draw original in red to debug alignment
    
    ofPushMatrix();
    ofTranslate(this->x, this->y);
    ofVec2f focusPos(x - this->x, y - this->y);
    
    ofPushStyle();
    
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
    
    
    // Then take care of the magnified line word by word path by path
    float xOffsetLeft = 0;
    float xOffsetRight = 0;

    float bigWordPadding = mWordBoundaryPadding * bigFontMult;
    
    std::vector<word*> wordsBefore;
    std::vector<word*> wordsAfter;
    std::vector<word*> wordsMagnify;

    std::string leftLettersStr = "";
    int leftWordX = 0;
    int rightWordX = 0;
    
    std::string rightLettersStr = "";
    std::string magnifyStr = "";
    
    ofVec2f magnifyPos = ofVec2f(0,0);
    ofRectangle magnifyBounding;
    
    bool before = true;
    
    float spaceWidthPx = ttf.getStringBoundingBox("a", 0, 0).width * 0.25;
    
    bool currentWordFound = false;
    
    for (std::size_t i = 0, eL = currentLine.size(); i != eL; ++i) {
        auto &w = currentLine.at(i);
        
        ofRectangle r(w->rect.x - (mWordBoundaryPadding + spaceWidthPx),
            w->rect.y - mLineHeight*2.25 - mWordBoundaryPadding,
            w->rect.width + (mWordBoundaryPadding + spaceWidthPx)*2,
            mLineHeight*4 + (mWordBoundaryPadding * 2));
        
        ofSetColor(255,100,255);
        /*ofNoFill();
        ofDrawRectangle(r);
        ofFill();
        */
        /*ofFill();
        ofSetColor(30,30,30);
        ttf.drawString(w->text, w->rect.x, w->rect.y);
        */
        
        if( r.inside(focusPos) && !currentWordFound) {
            currentWordFound = true;
            
            bool currentLetterFound = false;
            
            for (std::size_t ii = 0, e = w->letters.size(); ii != e; ++ii) {
                auto l = w->letters.at(ii);
                
                if( (!currentLetterFound && ( focusPos.x >= l.rect.getLeft()-spaceWidthPx || ii==0 ) && (focusPos.x <= l.rect.getRight()+spaceWidthPx || ii==w->letters.size() ))
                   ) {
                    
                    currentLetterFound = true;
                    
                    magnifyStr = l.text;
                    magnifyPos.y = w->rect.y + mWordBoundaryPadding ;
                    
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
                            magnifyStr = currentLine.at(wordIndex)->letters.at(letterIndex).text + magnifyStr;
                            letterCount++;
                            
                        } else {

                            // get word before
                            wordIndex--;
                            if(wordIndex < 0) {
                                break;
                            } else {
                                
                                letterIndex = currentLine.at(wordIndex)->letters.size();
                                magnifyStr = " " + magnifyStr;
                            }
                        }
                    }
                    
                    if(wordIndex >= 0) {
                        letterIndex--;
                        if(currentLine.at(wordIndex)->letters.size() - letterIndex > 0 ) {
                              for (int _i = letterIndex; _i >= 0; --_i) {
                                  leftLettersStr = currentLine.at(wordIndex)->letters.at(_i).text + leftLettersStr;
                              }
                            
                              leftWordX = currentLine.at(wordIndex)->rect.x;
                              wordIndex--;
                          }
                        
                          for (int _i = wordIndex; _i >= 0; --_i) {
                              wordsBefore.push_back( currentLine.at(_i) );
                          }
                    }
  
                    xOffsetLeft = ttfBig.getStringBoundingBox(magnifyStr, 0, 0).width;
                    magnifyPos.x = l.rect.getPosition().x - xOffsetLeft;
                    
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
                            magnifyStr = magnifyStr + currentLine.at(wordIndex)->letters.at(letterIndex).text;
                            letterIndex++;
                            letterCount++;
                            
                        } else {
                            // get word after
                            wordIndex++;
                            if(wordIndex >= currentLine.size()) {
                                break;
                            } else {
                                letterIndex = 0;
                                magnifyStr = magnifyStr + " ";
                            }
                        }
                    }
                    
                    if(wordIndex < currentLine.size()) {
                        
                        if( letterIndex <= currentLine.at(wordIndex)->letters.size() ) {
                                                      
                              for (int _i = letterIndex; _i < currentLine.at(wordIndex)->letters.size(); ++_i) {
                                  rightLettersStr = rightLettersStr + currentLine.at(wordIndex)->letters.at(_i).text;
                              }
                            
                              rightWordX = currentLine.at(wordIndex)->rect.x + (currentLine.at(wordIndex)->rect.width - ttf.getStringBoundingBox(rightLettersStr, 0, 0).width);
                              wordIndex++;
                          }
                        
                        for (int _i = wordIndex; _i < currentLine.size(); ++_i) {
                            wordsAfter.push_back( currentLine.at(_i) );
                        }
                    }
                    

                    
                    magnifyBounding = ttfBig.getStringBoundingBox(magnifyStr, magnifyPos.x, magnifyPos.y);
                    xOffsetRight = magnifyBounding.width - xOffsetLeft;
                    before = false;
                    
                // debug
                //ttf.drawString(letter.text, letter.rect.x, letter.rect.y);
                //ofDrawRectangle(l.rect);
                }
            }
            
                
        } else {
            /*if(before) {
                wordsBefore.push_back(w);
            } else {
                wordsAfter.push_back(w);
            }*/
        }
        
        
        
    }
    
    if(currentLine.size() > 0 ) {
        
        int linePos = currentLine.front()->rect.y;
           
           ofSetColor(30,30,30);

           for(auto &w : wordsBefore) {
               ttf.drawString(w->text, w->rect.x - xOffsetLeft, linePos);
           }
           
           if(leftLettersStr.size() > 0) {
               ttf.drawString(leftLettersStr, leftWordX - xOffsetLeft, linePos);
           }
           
           for(auto &w : wordsAfter) {
               ttf.drawString(w->text, w->rect.x + xOffsetRight, linePos);
           }
           
           if(rightLettersStr.size() > 0) {
               ttf.drawString(rightLettersStr, rightWordX + xOffsetRight, linePos);
           }
           
           
           // draw white bacground behind magnified
           ofSetColor(255,255,255);
           ofDrawRectangle(magnifyBounding);
           
           ofSetColor(0,0,0);
           ttfBig.drawString(magnifyStr, magnifyPos.x, magnifyPos.y);
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



void Paragraph::drawMagnified1(float x, float y, float scale) { // Scale is 1 to bigFontMult
    
    float maxDist = 400;
    float bigWordPadding = mWordBoundaryPadding * bigFontMult;
    ofVec2f focusPos(x - this->x, y - this->y);
    
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




