//
//  Paragraph.hpp
//  eye-read-01
//
//  Created by Johan Bichel Lindegaard on 26/03/2020.
//

#pragma once
#include <ofGraphics.h>
#include "ofTrueTypeFont.h"

class Paragraph{
    
    public:
    
        struct letter {
               std::string text;
               ofRectangle rect;
               ofPath path;
               
               float _xDist;
               float _yDist;
               float pushOut;
        };
    
        /*struct letterPath {
        
            ofPath path;
            std::string text;
            ofRectangle rect;
               
            float _xDist;
            float _yDist;
            float pushOut;
        };*/
    
        struct word {
            std::string text;
            ofRectangle rect;
            
            std::vector<ofPath> letterPaths;
            std::vector<letter> letters;
            
            //float xOffset;
            // Deprecate
            float _xDist;
            float _yDist;
            float pushOut;
        };
    

        enum Alignment
        {
            ALIGN_LEFT = 1,
            ALIGN_CENTER = 2,
            ALIGN_RIGHT = 3
        };
    
        Paragraph(std::string text = "Stumptown street art photo booth try-hard cold-pressed, pour-over raw denim four loko vinyl. Banjo drinking vinegar tousled, Brooklyn Neutra meggings mlkshk freegan whatever. Authentic drinking vinegar next level Portland tattooed, street art mixtape. Butcher High Life Brooklyn bicycle rights. Cardigan iPhone stumptown 90's, Carles Neutra viral Brooklyn ugh disrupt. Truffaut Williamsburg sriracha four dollar toast bicycle rights four loko. Migas Odd Future disrupt DIY polaroid whatever.", int width = 620, Alignment align = ALIGN_LEFT);
    
        int x;
        int y;
    
        void setText(std::string text);
        void setFont(std::string file, int ptSize);
    
        // void setMagScale() TODO
        //void setFont(std::shared_ptr<ofxSmartFont> font);
    
        void setColor(int color);
        void setColor(ofColor color);
    
        void setWidth(int width);
        void setIndent(int indent);
        void setPosition(int x, int y);
        void setAlignment(Alignment align);
        void setSpacing(int spacing);
        void setLeading(int leading);
        void setBorderPadding(int padding);
    
        int getWidth();
        int getHeight();
        int getStringHeight(std::string str = "");
        
        void drawLetterAtPos(float x, float y);
    
        void drawMagnified1(float x, float y, float scale=4);
        
        //
        void drawScrollingLine();
        void calculateScrollingLine(float x, float y, float rawx, float rawy);
        double scrollIn = 0;
        //
    
        void calculateAttractPoint(float x, float y);
    
    void calculateAttractPointScrolling(float x, float y);
    
        void calculateMagnifiedLetters(float x, float y, int numLettersLeft=4, int numLettersRight=15, bool pushLeft=true, bool magnifyWholeWords=true);
        void drawMagnifiedLetters(float x, float y, bool pushLeft=true, bool magnifyWholeWords=true);
    
        // void drawSegments
        ofVec2f attractPoint;
        ofVec2f focusPos;
    
        void getLetterCentroid(float x, float y);
        void drawNearestWord(float x, float y);

        void draw();
        void draw(int x, int y);
        void drawBorder(bool draw);
        void drawBorder(ofColor color);
        void drawWordBoundaries(bool draw = true);
    
    
    /*struct bigWord {
        std::string text;
        ofRectangle rect;
        float _xDist;
        float _yDist;
        float pushOut;
        
    };*/
    // TODO: optimization
    
        ofTrueTypeFont ttfBig;

        int DPI_SCALE_FACTOR = 2;
    
        int currentLineNumber = -1;
        
        bool isLastWord = false;
        bool freezeLastWord = false;
        
        bool nextLineTargetReached = false;
    
        int freezeLastWordDwellTime = 2000;
        int lineTransitionDwellTime = 10000;
    
        unsigned int long freezeLastWordTime = 0;

        int mLineHeight;

    private:
        
        int magnifyScale;
    
        int mWidth;
        int mHeight;
        int mIndent;
        int mLeading;
        int mSpacing;
        std::string mText;
        ofColor mColor;
        Alignment mAlignment;
    
        ofTrueTypeFont ttf;

        bool bDrawBorder;
        ofColor mBorderColor;
        int mBorderPadding;
        bool bDrawWordBoundaries;
        int mWordBoundaryPadding;
        ofColor mWordBoundaryColor;
    
        std::vector< letter > mLetters;
        //std::vector< letterPath > mLetterPaths;
    
        std::vector< word > mWords;
        std::vector< std::vector<word*> > mLines;
        std::vector< word*> currentLine;
    
        std::vector< word*> nextLine;

    float currentLineWidth;
    
        word* currentWord;
    
        // animation variables
        float next_xOffsetLeft = 0;
        float next_xOffsetRight = 0;
    
        float draw_xOffsetLeft = 0;
        float draw_xOffsetRight = 0;
    
        std::vector<word*> next_wordsBefore;
        std::vector<word*> next_wordsAfter;
        //std::vector<word*> next_wordsMagnify;
    
        std::vector<word*> draw_wordsBefore;
        std::vector<word*> draw_wordsAfter;
        //std::vector<word*> draw_wordsMagnify;
        
        int next_leftWordX = 0;
        int next_rightWordX = 0;
        int draw_leftWordX = 0;
        int draw_rightWordX = 0;
    
        std::string next_leftLettersStr = "";
        std::string next_rightLettersStr = "";
        std::string next_magnifyStr = "";
        ofVec2f next_magnifyPos = ofVec2f(0,0);
    
     ofRectangle next_magnifyBounding;
    
        std::string draw_leftLettersStr = "";
        std::string draw_rightLettersStr = "";
        std::string draw_magnifyStr = "";
        ofVec2f draw_magnifyPos = ofVec2f(0,0);
    
     ofRectangle draw_magnifyBounding;
    
    float spaceWidthPx;
    
        void render();
        inline void drawLeftAligned();
        inline void drawCenterAligned();
        inline void drawRightAligned();
    
        static inline std::string &trim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) {return !isspace(c); }));
            s.erase(std::find_if(s.rbegin(), s.rend(), [](char c) {return !isspace(c); }).base(), s.end());
            return s;
        }
};
