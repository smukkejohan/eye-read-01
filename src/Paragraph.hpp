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
    
        void calculateAttractPoint(float x, float y);

        void drawMagnifiedLetters(float x, float y, int numLettersLeft=4, int numLettersRight=15, bool pushLeft=true, float scale=4);
    
        ofVec2f attractPoint;
    
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
        
    };*/ // TODO: optimization
        
    
    private:
        
        int bigFontMult;
    
        int mWidth;
        int mHeight;
        int mIndent;
        int mLeading;
        int mSpacing;
        int mLineHeight;
        std::string mText;
        ofColor mColor;
        Alignment mAlignment;
    
        ofTrueTypeFont ttf;
        ofTrueTypeFont ttfBig;

    
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




