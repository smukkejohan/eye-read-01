
#include "ofApp.h"

float TobiiX = 0;
float TobiiY = 0;

static zmq::context_t *ctx = NULL;
static zmq::context_t& ZmqContext()
{
    if (ctx == NULL)
    {
        ctx = new zmq::context_t(4);
    }
    return *ctx;
}

void PupilZmq::connect(string addr="127.0.0.1") {
    
    zmq::socket_t req_socket(ZmqContext(), ZMQ_REQ);
    req_socket.connect(getApiAddress(addr, 50020));
    
    const std::string cmd = "SUB_PORT";
    zmq::message_t cmdrequest (cmd.size());
    memcpy (cmdrequest.data (), cmd.c_str(), cmd.size());
    req_socket.send(cmdrequest);
    
    zmq::message_t reply;
    req_socket.recv(&reply);
    std::cout << "Received SUB_PORT port: " << reply.to_string() << std::endl;
    
    int port = std::stoi( reply.to_string() );
    socket = zmq::socket_t(ZmqContext(), ZMQ_SUB);
    socket.connect(getApiAddress(addr, port));
    
    // https://docs.pupil-labs.com/developer/core/overview/#pupil-datum-format
    const std::string topic = "pupil.0"; // pupil.0";//"gaze.3d.1.";
    socket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    
    std::cout << "ZMQ_SUBSCRIBE" << std::endl;
}

void PupilZmq::receive() {
        
    //ZMQ_SNDMORE ??
    // how to make sure we empty the buffer ?
    zmq::message_t frame1;
    zmq::message_t frame2;
    //socket.get(sockopt::integral_option<Opt, T, BoolUnit>)
    //socket.get(<#sockopt::array_option<Opt, NullTerm>#>)
    
    int64_t hwm;
    size_t hwm_size = sizeof (hwm);
    //zmq_getsockopt (socket, ZMQ, &hwm, &hwm_size);
    //auto opt = socket.get(zmq::sockopt::events)
    
    while( socket.get(zmq::sockopt::events) & ZMQ_POLLIN ) { // zmq::sockopt::pollin
        socket.recv(&frame1);
        socket.recv(&frame2);
        // frame2 is a msgpack encoded key-value mapping
    }
    
    try {
        
        msgpack::sbuffer sbuf;
           sbuf.write(static_cast<const char *>(frame2.data()), frame2.size());

           // deserialize it.
           msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());
           
           // deserialized object is valid during the msgpack::object_handle instance is alive.
           msgpack::object deserialized = oh.get();

           //std::cout << "Received " << frame1 << deserialized << std::endl;
           
           // convert msgpack::object instance into the original type.
           // if the type is mismatched, it throws msgpack::type_error exception.
        
            Pupil newData;
           deserialized.convert(newData);
            
        if(newData.confidence > 0.6) {
           // std::cout << "Pupil " << pupil.norm_pos[0] << "," << pupil.norm_pos[1] << "  " << pupil.confidence << std::endl;
            pupil = newData;
        }
        
    } catch(const std::exception& e) {
        
    }
       
    
}


void ofApp::setup()
{
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetFrameRate(60);
    
    gui.setup();
    gui.add(bUseEyeTracker.set("use eye tracker", true));
    
    gui.add(pushText.set("push", true));
    
    
    gui.add(mode.set("Mode", 1, 0, 1));
    
    gui.add(filterFc.set("filterFc", 0.011));
    gui.add(filterQ.set("filterQ", 0.707));
    
    zoom_mode_params.setName("Magnified reading");
        
    zoom_mode_params.add(magnifyWholeWords.set("Magnify whole words", true));
    zoom_mode_params.add(numLettersLeft.set("Magnify letters left", 4, 0, 20));
    zoom_mode_params.add(numLettersRight.set("Magnify letters right", 15, 0, 30));
    
    zoom_mode_params.add(lineChangePreviousDwellMs.set("Dwell previous line (ms)", 1200, 0, 6000));
    zoom_mode_params.add(lineChangeNextDwellMs.set("Dwell next line (ms)", 600, 0, 3000));
    zoom_mode_params.add(freezeLastWordDwellTime.set("Dwell last word (ms)", 2000, 0, 4000));
    zoom_mode_params.add(lineTransitionDwellTime.set("Dwell line progression (ms)", 10000, 4000, 20000));
    
    gui.add(zoom_mode_params);
    
    hint_mode_params.setName("Returned Gaze Hint");
    hint_mode_params.add(hintDurationMs.set("Duration (ms)", 2000, 10, 8000));
    
    hint_mode_params.add(hintExtendBack.set("Extend lines back", 1, 0, 4));
    hint_mode_params.add(hintExtendForward.set("Extend lines forward", 1, 0, 4));
    
    gui.add(hint_mode_params);


    
    /*gui.add(magnifyScale.set("mag scale", 4, 1.0, 8.0));
    magnifyScale.addListener(this, &ofApp::magScaleChanged);
    */
    
    gui.add(showCursor.set("Draw gaze point", true));
    gui.add(showAttractPoint.set("Draw attract point", true));
    
    //gui.add(radius.set("radius", radius));
    //gui.add(magnificationArea.set("magnify area", magnificationArea));
    //pupilZmq.connect();
    
    if(bUseEyeTracker) {
        try {
            tobii.connect();
        } catch(const char* msg) {
            std::cout<<msg<<" Using mouse position as input."<<std::endl;
            bUseEyeTracker.set(false);
        }
    }
    
    ofEnableAlphaBlending();
    
    //ofDisableArbTex();
    //ofSetOrientation(OF_ORIENTATION_DEFAULT, false);
    
    //fbo1.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    //shader.load("shadersGL3/shader");
    //shader.load("shadersGL3/magglass");
    
    /*int planeWidth = ofGetScreenWidth();
    int planeHeight = ofGetScreenHeight();
    int planeGridSize = 20;
    int planeColums = planeWidth / planeGridSize;
    int planeRows = planeHeight / planeGridSize;
    
    plane.set(planeWidth, planeHeight, planeColums, planeRows, OF_PRIMITIVE_TRIANGLES);
    plane.mapTexCoordsFromTexture(fbo1.getTexture());*/
    
    ofBackground(ofColor::white);
    ofSetFullscreen(true);
    
    ofBuffer textBuffer = ofBufferFromFile(ofToDataPath("text.txt"), false);
    
    Paragraph* p = new Paragraph(textBuffer.getText());
    
        p->setColor(ofColor::fromHex(0x555555));
        //p->drawBorder(ofColor::fromHex(0x777777));
        //p->drawWordBoundaries();
        p->setAlignment(Paragraph::ALIGN_LEFT);
       
        paragraphs.push_back(p);

// change these to whatever you want //
    int pWidth = 1400;
    int pFontSize = 14;
    float pPadding = pWidth*.30;

// check for high resolution display //
    if (ofGetScreenWidth()>=2560 && ofGetScreenHeight()>=1600)
    {
        pWidth*=2;
        pFontSize*=2;
    }
    
// load our fonts and layout our paragraphs //
    paragraphs[0]->setFont("Helvetica", pFontSize); // Set font here
    //ofxSmartFont::list();
    
    int pLeading = pFontSize*1.3; //.85; // 0.65 // // Set line spacing here
    int tWidth = paragraphs.size()*pWidth + (paragraphs.size()-1)*pPadding;
    
    int x = (ofGetWidth() - tWidth)/2;
    for (int i=0; i<paragraphs.size(); i++){
        paragraphs[i]->setWidth(pWidth);
        paragraphs[i]->setLeading(pLeading);
        paragraphs[i]->setSpacing(pFontSize*.7);
        paragraphs[i]->setBorderPadding(30);
        paragraphs[i]->setPosition(x+((pWidth+pPadding)*i), ofGetHeight()/2 - paragraphs[0]->getHeight()/2);
    }
    
    file.open(ofToDataPath("data-export-"+ofGetTimestampString()+".txt"), ofFile::ReadWrite);
    file.create();
    
    stringstream data;
    data<<"timestamp;rawx;rawy;filteredx;filteredy;currentLineNumber"<<std::endl;
    file.writeFromBuffer(ofBuffer(data));
    
    std::cout << "end setup" << std::endl;
}


void ofApp::update() {
    
    if(bUseEyeTracker) {
        //std::cout<<TobiiX<<":"<<TobiiY<<std::endl;
        //pupilZmq.receive();
        
        tobii.get_data();
        if(isnan(TobiiX) || isnan(TobiiY)) {
            // No update, person is probably blinking
        } else {
            rawx = TobiiX * ofGetWidth();
            rawy = TobiiY * ofGetHeight();
        }
        
    } else {
        rawx = mouseX;
        rawy = mouseY;
    }

    filter.setFc(filterFc);
    filter.setQ(filterQ);
    
    
    if(mode.get() == ZOOM_READING_MODE) {
    
        paragraphs[0]->freezeLastWordDwellTime = freezeLastWordDwellTime.get();
        paragraphs[0]->lineTransitionDwellTime = lineTransitionDwellTime.get();
        
        rawx = ofClamp(rawx, paragraphs[0]->x, paragraphs[0]->x+paragraphs[0]->getWidth());
        rawy = ofClamp(rawy, paragraphs[0]->y - (paragraphs[0]->mLineHeight * 3), paragraphs[0]->y+paragraphs[0]->getHeight());
    
        paragraphs[0]->calculateAttractPointScrolling(rawx, rawy);
    
        if(paragraphs[0]->attractPoint.y > yTarget) {
            if (ofGetElapsedTimeMillis() - lineChangeTimeNext > lineChangeNextDwellMs) {
                
                // After dwell time continue to next line
                yTarget = paragraphs[0]->attractPoint.y;
            }
            
        } else {
            // reset if still in in same line
            lineChangeTimeNext = ofGetElapsedTimeMillis();
        }
        
        if(rawy < yTarget - (paragraphs[0]->mLineHeight*2)) {
            if (ofGetElapsedTimeMillis() - lineChangeTimePrevious > lineChangePreviousDwellMs ) {
                // After dwell time return to previous line
                yTarget = paragraphs[0]->attractPoint.y;
            }
        } else {
            // reset if still in in same line
            lineChangeTimePrevious = ofGetElapsedTimeMillis();
        }
        
        filter.update(ofVec2f( paragraphs[0]->attractPoint.x, yTarget ));
        
        x = filter.value().x;
        y = filter.value().y;
        
        //paragraphs[0]->calculateMagnifiedLetters(x, y, numLettersLeft, numLettersRight, pushText, magnifyWholeWords);
        paragraphs[0]->calculateScrollingLine(x, y, rawx, rawy);
        
        // TODO: sampling rate setting
        // semi colon and linebreak seperated in a txt file
        // name file with timestamp
        // "timestamp: rawx, rawy, filteredx, filteredy, currentline"
        
    } else if(mode == RETURN_HINT_MODE) {
        
        if(
           (rawx < paragraphs[0]->x || rawx > paragraphs[0]->x + paragraphs[0]->getWidth()) ||
           (rawy < paragraphs[0]->y - paragraphs[0]->mLineHeight || rawy > paragraphs[0]->y + paragraphs[0]->getHeight())
           
           ) {
            
            if(!lookAway) {
                lookAway = true;
                lookBackOrAwayTime = ofGetElapsedTimeMillis();
                lastLookAtPosition = filter.value(); // TODO: we might need to save a value from a short moment before ? or is filter enough?
            }
            
            
        } else {
            
            if(lookAway) {
                lookAway = false;
                lookBackOrAwayTime = ofGetElapsedTimeMillis();
            }
            
            paragraphs[0]->calculateAttractPointScrolling(rawx, rawy);
            filter.update(ofVec2f( paragraphs[0]->attractPoint.x, paragraphs[0]->attractPoint.y ));
            x = filter.value().x;
            y = filter.value().y;
            
        }
        
    }
    
    stringstream data;
    data<<ofGetTimestampString()<<";"<<rawx<<";"<<rawy<<";"<<x<<";"<<y<<";"<<paragraphs[0]->currentLineNumber<<std::endl;
    file.writeFromBuffer(ofBuffer(data));
    
}

void ofApp::draw()
{
    
    if(mode.get() == ZOOM_READING_MODE) {
    // std::cout << "draw" << std::endl;
    // draw the paragraphs
    // how to get word and letter at fixation point?
    // variable:  enlarge 7 to 15 characters in front of fixation point
    // decrease magnification of text before
    // gradual speed
    // keep enlarged
    
    // text in box or gradual white out line above and under
    //float cx = ofGetWidth() / 2.0;
    //float cy = ofGetHeight() / 2.0;
    
    // the plane is being position in the middle of the screen,
    // so we have to apply the same offset to the mouse coordinates before passing into the shader.
    // TODO add gui toggle
    //x = ofMap(pupilZmq.pupil.norm_pos[0], LEFT_X, RIGHT_X, 0, ofGetWidth());
    //y = ofMap(pupilZmq.pupil.norm_pos[1], TOP_Y, BOTTOM_Y, 0, ofGetHeight());
    
    //x = (pupilZmq.pupil.norm_pos[0] * ofGetWidth()) ; //- cx; //
    //y = ((1-pupilZmq.pupil.norm_pos[1]) * ofGetHeight()) ; //- cy; //
    
    //x = pupilZmq.pupil.norm_pos[0] * ofGetWidth();
    //y = (1-pupilZmq.pupil.norm_pos[1]) * ofGetHeight();
    //std::cout << "Data  " << pupilZmq.pupil.norm_pos[0] << ", " << pupilZmq.pupil.norm_pos[1] << std::endl;
    
    //x = mouseX;
    //y = mouseY;
    
    //fbo1.begin();
        ofClear(255, 255, 255, 255);

    //ofPushMatrix();
    //ofScale(1,-1, 1);
        //for (int i=0; i<paragraphs.size(); i++){
        ofSetColor(255);
        //paragraphs[0]->draw();
        //paragraphs[0]->drawMagnified1(x, y, 4);
        //paragraphs[0]->drawMagnifiedLetters(x, y, pushText, magnifyWholeWords);
        
        paragraphs[0]->drawScrollingLine();
        
    //paragraphs[0]->draw();
        //ofDrawCircle(x, y, 10);

        //}
    //ofPopMatrix();
    //fbo1.end();
    
    //shader.begin();
        
   // shader.setUniform4f("iMouse",  x-cx, -(y-cy), 0, 0);
   // shader.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        
       //shader.setUniform1f("mouseRange", radius*150);
       //shader.setUniform2f("magnificationArea", magnificationArea*150);
       //shader.setUniform2f("mousePos", x-cx, -(y-cy));
       //shader.setUniformTexture("tex0", fbo1.getTexture(), 0);
        // send distance to top of line
        // send lineheight
        // send bounds of current word
        // send bounds of current line
    
    /*ofPushMatrix();
        ofTranslate(cx, cy);
        //ofScale(0.25,-0.25);
        //ofScale(1,-1);
        //plane.draw();
        
    ofPopMatrix();
     */
    //fbo1.draw(0, 0);
    //shader.end();
        
    } else if(mode == RETURN_HINT_MODE) {
        ofClear(255, 255, 255, 255);
        ofSetColor(255);
        
        float hintP = 0;
        unsigned int elapsed = ofGetElapsedTimeMillis() - lookBackOrAwayTime;
        
        if(elapsed < hintDurationMs.get() && !lookAway ) {
            // Just looked back
            hintP = ofClamp(ofMap(elapsed, 0, hintDurationMs.get(), 1.0, 0.0), 0.0, 1.0);
            
        }
        
        //std::cout<<hintP<<std::endl;
        paragraphs[0]->drawHintHighlight(lastLookAtPosition, hintP, hintExtendBack.get(), hintExtendForward.get());
        //paragraphs[0]->drawHintHighlightSentences(lastLookAtPosition, hintP, hintExtendBack.get(), hintExtendForward.get());
        
    }
    
    ofPushStyle();
    if(showCursor) {
        ofSetColor(255,120,120, 127);
        ofDrawCircle(x, y, 8);
        
        ofSetColor(120,120,255, 127);
        ofDrawCircle(rawx, rawy, 6);

    }
            
    if(showAttractPoint) {
        ofSetColor(120,255,120, 127);
        ofDrawCircle(paragraphs[0]->attractPoint, 4);
    }
    
    ofPopStyle();
    
    ofSetColor(0);
    ofDrawBitmapString(ofGetFrameRate(), ofGetWidth()-80, ofGetHeight()-40);
    
    ofSetColor(255);
    gui.draw();
    
    ofSetColor(0);

    /*ofDrawRectangle(paragraphs[0]->x, paragraphs[0]->y, paragraphs[0]->getWidth(), paragraphs[0]->getHeight());*/
    
    
    
}




