
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

void ofApp::audioOut(ofSoundBuffer & buffer){
    //pan = 0.5f;
    float leftScale = 1 - pan;
    float rightScale = pan;

    // sin (n) seems to have trouble when n is very large, so we
    // keep phase in the range of 0-TWO_PI like this:
    while (phase > TWO_PI){
        phase -= TWO_PI;
    }

    if ( audioHintNoise.get() == true){
        // ---------------------- noise --------------
        for (size_t i = 0; i < buffer.getNumFrames(); i++){
            lAudio[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom(0, 1) * volume * leftScale;
            rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = ofRandom(0, 1) * volume * rightScale;
        }
    } else {
        phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
        for (size_t i = 0; i < buffer.getNumFrames(); i++){
            phase += phaseAdder;
            float sample = sin(phase);
            lAudio[i] = buffer[i*buffer.getNumChannels()    ] = sample * volume * leftScale;
            rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = sample * volume * rightScale;
        }
    }

}


void ofApp::setup()
{
    
    
    int bufferSize        = 512;
    sampleRate             = 44100;
    phase                 = 0;
    phaseAdder             = 0.0f;
    phaseAdderTarget     = 0.0f;
    volume                = 0.1f;
    
    lAudio.assign(bufferSize, 0.0);
    rAudio.assign(bufferSize, 0.0);
    
    ofSoundStreamSettings settings;
    
#ifdef TARGET_LINUX
    // Latest linux versions default to the HDMI output
    // this usually fixes that. Also check the list of available
    // devices if sound doesn't work
    auto devices = soundStream.getMatchingDevices("default");
    if(!devices.empty()){
        settings.setOutDevice(devices[0]);
    }
#endif
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);

    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetFrameRate(60);
        
    all_params.add(bUseEyeTracker.set("use eye tracker", true));
    //gui.add(pushText.set("push", true));
    all_params.add(textParam.set("Text", text1));
    all_params.add(mode.set("Mode", 1, 0, 1));
    all_params.add(filterFc.set("filterFc", 0.025));
    all_params.add(filterQ.set("filterQ", 0.5));
    zoom_mode_params.setName("Magnified reading");
    //zoom_mode_params.add(magnifyWholeWords.set("Magnify whole words", true));
    //zoom_mode_params.add(numLettersLeft.set("Magnify letters left", 4, 0, 20));
    //zoom_mode_params.add(numLettersRight.set("Magnify letters right", 15, 0, 30));
    zoom_mode_params.add(lineChangePreviousDwellMs.set("Dwell previous line (ms)", 2500, 0, 6000));
    zoom_mode_params.add(lineChangeNextDwellMs.set("Dwell next line (ms)", 600, 0, 3000));
    //zoom_mode_params.add(freezeLastWordDwellTime.set("Dwell last word (ms)", 2000, 0, 4000));
    zoom_mode_params.add(lineTransitionDwellTime.set("Dwell line progression (ms)", 6000, 4000, 20000));
    
    //zoom_mode_params.add(magXFilterFc.set("mag filterFc", 0.011));
    //zoom_mode_params.add(magXFilterQ.set("mag filterQ", 0.707));
    
    all_params.add(zoom_mode_params);
    
    hint_mode_params.setName("Returned Gaze Hint");
    hint_mode_params.add(hintDurationMs.set("Duration (ms)", 2000, 0, 10000));
    hint_mode_params.add(hintAudioDurationMs.set("Duration audio (ms)", 2000, 0, 10000));

    hint_mode_params.add(hintLookAwayThresholdMs.set("Look away threshold", 1200, 0, 10000));
    
    hint_mode_params.add(hintLookAwayDeadzonePadding.set("Deadzone padding", 200, 0, 400));

    
    hint_mode_params.add(hintExtendBack.set("Extend lines back", 1, 0, 4));
    hint_mode_params.add(hintExtendForward.set("Extend lines forward", 1, 0, 4));
    
    hint_mode_params.add(audioHintAmp.set("Audio amplitude", 0.1f, 0, 1));
    hint_mode_params.add(audioHintNoise.set("Audio noise", false));
    hint_mode_params.add(audioHintVariableFreq.set("Audio var freq", true));
    hint_mode_params.add(audioHintTargetFreq.set("Audio freq", 800, 20, 20000));

    all_params.add(hint_mode_params);
    
    /*gui.add(magnifyScale.set("mag scale", 4, 1.0, 8.0));
    magnifyScale.addListener(this, &ofApp::magScaleChanged);
    */
    
    all_params.add(showCursor.set("Draw gaze point", true));
    all_params.add(showAttractPoint.set("Draw attract point", true));
    
    //gui.add(radius.set("radius", radius));
    //gui.add(magnificationArea.set("magnify area", magnificationArea));
    //pupilZmq.connect();
    
    loadSettings("settings-1");
    
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
    
    //loadText();

    Paragraph* p = new Paragraph(textParam.get());
    
        p->setColor(ofColor::fromHex(0x555555));
        //p->drawBorder(ofColor::fromHex(0x777777));
        //p->drawWordBoundaries();
        p->setAlignment(Paragraph::ALIGN_LEFT);
       
        paragraph = p;

// change these to whatever you want //
    int pWidth = ofGetScreenWidth() * 0.6;
    int pFontSize = 14;
    float pPadding = pWidth*.30;

// check for high resolution display //
    if (ofGetScreenWidth()>=2560 && ofGetScreenHeight()>=1600)
    {
        pWidth*=2;
        pFontSize*=2;
    }
    
// load our fonts and layout our paragraphs //
    paragraph->setFont("Helvetica", pFontSize); // Set font here
    //ofxSmartFont::list();
    
    int pLeading = pFontSize*1.3; //.85; // 0.65 // // Set line spacing here
    int tWidth = pWidth; // + pPadding;
    
    int x = (ofGetWidth() - tWidth)/2;
    
    paragraph->setWidth(pWidth);
    paragraph->setLeading(pLeading);
    paragraph->setSpacing(pFontSize*.7);
    paragraph->setBorderPadding(30);
    paragraph->setPosition(x, ofGetHeight()/2 - paragraph->getHeight()/2);
    
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
    
        paragraph->freezeLastWordDwellTime = freezeLastWordDwellTime.get();
        paragraph->lineTransitionDwellTime = lineTransitionDwellTime.get();
        
        rawx = ofClamp(rawx, paragraph->x, paragraph->x+paragraph->getWidth());
        rawy = ofClamp(rawy, paragraph->y - (paragraph->mLineHeight * 3), paragraph->y+paragraph->getHeight());
    
        paragraph->calculateAttractPointScrolling(rawx, rawy);
    
        if(paragraph->attractPoint.y > yTarget) {
            if (ofGetElapsedTimeMillis() - lineChangeTimeNext > lineChangeNextDwellMs) {
                
                // After dwell time continue to next line
                yTarget = paragraph->attractPoint.y;
            }
            
        } else {
            // reset if still in in same line
            lineChangeTimeNext = ofGetElapsedTimeMillis();
        }
        
        if(rawy < yTarget - (paragraph->mLineHeight*2)) {
            if (ofGetElapsedTimeMillis() - lineChangeTimePrevious > lineChangePreviousDwellMs ) {
                // After dwell time return to previous line
                yTarget = paragraph->attractPoint.y;
            }
        } else {
            // reset if still in in same line
            lineChangeTimePrevious = ofGetElapsedTimeMillis();
        }
        
        filter.update(ofVec2f( paragraph->attractPoint.x, yTarget ));
        
        x = filter.value().x;
        y = filter.value().y;
        
        //paragraph->calculateMagnifiedLetters(x, y, numLettersLeft, numLettersRight, pushText, magnifyWholeWords);
        paragraph->calculateScrollingLine(x, y, rawx, rawy);
        
        // TODO: sampling rate setting
        // semi colon and linebreak seperated in a txt file
        // name file with timestamp
        // "timestamp: rawx, rawy, filteredx, filteredy, currentline"
        
    } else if(mode == RETURN_HINT_MODE) {
        int pad = hintLookAwayDeadzonePadding.get();
        
        unsigned int t = ofGetElapsedTimeMillis();

        
        if(
           (rawx < paragraph->x - pad || rawx > paragraph->x + paragraph->getWidth() + pad ) ||
           (rawy < paragraph->y - paragraph->mLineHeight - pad || rawy > paragraph->y + paragraph->getHeight() + pad)
           
           ) {
            
            if(!lookAway) {
                lookAway = true;
                lookBackOrAwayTime = t;
                lastLookAtPosition = filter.value(); // TODO: we might need to save a value from a short moment before ? or is filter enough?
                
                int width = paragraph->getWidth();
                pan = (float)lastLookAtPosition.x / (float)width;
                //float height = (float)ofGetHeight();
                //float heightPct = ((height-lastLookAtPosition.y) / height);
                
                // conditional frequency map
                if(audioHintVariableFreq.get()) {
                    targetFrequency = audioHintTargetFreq.get() * pan;
                } else {
                    targetFrequency = audioHintTargetFreq.get();
                }
                
                phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
            }
            
            
            unsigned int elapsed = t - lookBackOrAwayTime;
            if(elapsed > hintLookAwayThresholdMs.get() && !hintActive) {
                hintActive = true;
                hintActiveTime = t;
            }
            
            
        } else {
            
            if(lookAway) {
                
                if(
                   rawx > paragraph->x && rawx < paragraph->x + paragraph->getWidth() &&
                   rawy > paragraph->y && rawy < paragraph->y + paragraph->getHeight()
                   ) {
                    
                    lookAway = false;
                    lookBackOrAwayTime = t;
                    
                }
                    
                    

            } else {
                
                unsigned int elapsed = t - lookBackOrAwayTime;
                unsigned int elapsedHint = t - hintActiveTime;

                if(elapsed > hintDurationMs.get() && elapsed > hintAudioDurationMs.get()) {
                    hintActive = false;
                }
                
            }
            

            
            paragraph->calculateAttractPointScrolling(rawx, rawy);
            filter.update(ofVec2f( paragraph->attractPoint.x, paragraph->attractPoint.y ));
            x = filter.value().x;
            y = filter.value().y;
            
            
        }
    }
    
    stringstream data;
    data<<ofGetTimestampString()<<";"<<rawx<<";"<<rawy<<";"<<x<<";"<<y<<";"<<paragraph->currentLineNumber<<std::endl;
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
        //paragraph->draw();
        //paragraph->drawMagnified1(x, y, 4);
        //paragraph->drawMagnifiedLetters(x, y, pushText, magnifyWholeWords);
        
        paragraph->drawScrollingLine();
        
    //paragraph->draw();
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
        float hintPAudio = 0;
        
        unsigned int t = ofGetElapsedTimeMillis();
        
        unsigned int elapsed = t - lookBackOrAwayTime;
        unsigned int elapsedHint = t - hintActiveTime;

        if(hintActive ) {
            if(lookAway) {
                hintP = 1;
            } else {
                if(elapsed < hintDurationMs.get() ) {
                    // Just looked back
                    hintP = ofClamp(ofMap(elapsed, 0, hintDurationMs.get(), 1.0, 0.0), 0.0, 1.0);
                    
                }
                
                if(elapsed < hintAudioDurationMs.get() ) {
                    // Just looked back
                    hintPAudio = ofClamp(ofMap(elapsed, 0, hintAudioDurationMs.get(), 1.0, 0.0), 0.0, 1.0);
                    
                }
                
            }
            

            
        }
        

        

        
        volume = hintPAudio * audioHintAmp.get();
        
        //std::cout<<hintP<<std::endl;
        paragraph->drawHintHighlight(lastLookAtPosition, hintP, hintExtendBack.get(), hintExtendForward.get());
        //paragraph->drawHintHighlightSentences(lastLookAtPosition, hintP, hintExtendBack.get(), hintExtendForward.get());
        
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
        ofDrawCircle(paragraph->attractPoint, 4);
    }
    
    ofPopStyle();
    
    ofSetColor(0);
    ofDrawBitmapString(ofGetFrameRate(), ofGetWidth()-80, ofGetHeight()-40);
    
    ofDrawBitmapString("l", 10, ofGetHeight()-40);
    ofDrawBitmapString(lookAway, 20, ofGetHeight()-40);
    
    ofDrawBitmapString("h", 10, ofGetHeight()-80);
    ofDrawBitmapString(hintActive, 20, ofGetHeight()-80);

    
    ofSetColor(255);
    if(drawGui) {
        gui.draw();
    }
    
    ofSetColor(0);

    /*ofDrawRectangle(paragraph->x, paragraph->y, paragraph->getWidth(), paragraph->getHeight());*/
}

void ofApp::keyPressed(int key){

}

void ofApp::loadSettings(string name) {
    gui.setup(name, name + ".xml");
    gui.add(all_params);
    gui.loadFromFile(name + ".xml");
}

void ofApp::loadText() {
    paragraph->setText(textParam.get());
}

void ofApp::keyReleased(int key){
    
    switch(key) {
        case 'x':
            drawGui = !drawGui;
            break;
        case '1':
            loadSettings("settings-1");
            loadText();
            break;
        case '2':
            loadSettings("settings-2");
            loadText();
            break;
        case '3':
            loadSettings("settings-3");
            loadText();
            break;
        case '4':
            loadSettings("settings-4");
            loadText();
            break;
        case '5':
            loadSettings("settings-5");
            loadText();
            break;
        case '6':
            loadSettings("settings-6");
            loadText();
            break;
        case '7':
            loadSettings("settings-7");
            loadText();
            break;
        case '8':
            loadSettings("settings-8");
            loadText();
            break;
        case '9':
            loadSettings("settings-9");
            loadText();
            break;
        default:
            break;
    }
    
}

void ofApp::exit()
{
    soundStream.close();
}
