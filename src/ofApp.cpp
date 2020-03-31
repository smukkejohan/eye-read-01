
//
//  ofxParagraphExample
//
//  Created by Stephen Braitsch on 9/17/15
//  https://github.com/braitsch/ofxParagraph
//

#include "ofApp.h"


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
    
    const std::string topic = "gaze.3d.1.";
    socket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    
    
    std::cout << "ZMQ_SUBSCRIBE" << std::endl;
}


void PupilZmq::receive() {
    
       zmq::message_t frame1;
       socket.recv(&frame1);
       zmq::message_t frame2;
       socket.recv(&frame2);
       // frame2 is a msgpack encoded key-value mapping

       msgpack::sbuffer sbuf;
       sbuf.write(static_cast<const char *>(frame2.data()), frame2.size());

       // deserialize it.
       msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());
       
       // deserialized object is valid during the msgpack::object_handle instance is alive.
       msgpack::object deserialized = oh.get();

       //std::cout << "Received " << frame1 << deserialized << std::endl;
       
       // convert msgpack::object instance into the original type.
       // if the type is mismatched, it throws msgpack::type_error exception.
       deserialized.convert(gaze);
       //std::cout << "Gaze " << gaze.norm_pos[0] << ":" << gaze.norm_pos[1] << std::endl;
}


void ofApp::setup()
{
    
    pupilZmq.connect();
    
    

    ofEnableAlphaBlending();
    //ofDisableArbTex();
    //ofSetOrientation(OF_ORIENTATION_DEFAULT, false);
    
    fbo1.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    
    shader.load("shadersGL3/shader");
    
    int planeWidth = ofGetWidth();
    int planeHeight = ofGetHeight();
    int planeGridSize = 20;
    int planeColums = planeWidth / planeGridSize;
    int planeRows = planeHeight / planeGridSize;
    
    plane.set(planeWidth, planeHeight, planeColums, planeRows, OF_PRIMITIVE_TRIANGLES);
    
    plane.mapTexCoordsFromTexture(fbo1.getTexture());

    
    ofBackground(ofColor::white);
    ofSetFullscreen(true);

        Paragraph* p = new Paragraph(text1);
        p->setColor(ofColor::fromHex(0x555555));
        //p->drawBorder(ofColor::fromHex(0x777777));
        //p->drawWordBoundaries();
        p->setAlignment(Paragraph::ALIGN_LEFT);
       
        paragraphs.push_back(p);

// change these to whatever you want //
    int pWidth = ofGetScreenWidth() - 600;
    int pFontSize = 14;
    float pPadding = pWidth*.30;

// check for high resolution display //
    if (ofGetScreenWidth()>=2560 && ofGetScreenHeight()>=1600)
    {
        pWidth*=2;
        pFontSize*=2;
    }
    
// load our fonts and layout our paragraphs //
    paragraphs[0]->setFont("Helvetica", pFontSize);
    //ofxSmartFont::list();
    
    int pLeading = pFontSize*.65;
    int tWidth = paragraphs.size()*pWidth + (paragraphs.size()-1)*pPadding;
    
    int x = (ofGetWidth() - tWidth)/2;
    for (int i=0; i<paragraphs.size(); i++){
        paragraphs[i]->setWidth(pWidth);
        paragraphs[i]->setLeading(pLeading);
        paragraphs[i]->setSpacing(pFontSize*.7);
        paragraphs[i]->setBorderPadding(30);
        paragraphs[i]->setPosition(x+((pWidth+pPadding)*i), ofGetHeight()/2 - paragraphs[0]->getHeight()/2);
    }
    
    std::cout << "end setup" << std::endl;

}


void ofApp::update() {
    
    std::cout << "update" << std::endl;
    
    pupilZmq.receive();
    


    /*
           
    */
}

void ofApp::draw()
{
    
    //std::cout << "draw" << std::endl;

    
    // draw the paragraphs //
    // how to get word and letter at fixation point?
    
    //variable:  enlarge 7 to 15 characters in front of fixation point
    // decrease magnification of text before
    // gradual speed
    // keep enlarged
    
    // text in box or gradual white out line above and under
    
    fbo1.begin();
        ofClear(255, 255, 255, 255);
        ofSetColor(255);
        
    ofPushMatrix();
    //ofScale(1,-1, 1);
        for (int i=0; i<paragraphs.size(); i++){
            paragraphs[i]->draw();
        }
    ofPopMatrix();
    fbo1.end();
    
    shader.begin();
       
       // center screen.
       float cx = ofGetWidth() / 2.0;
       float cy = ofGetHeight() / 2.0;
       
       // the plane is being position in the middle of the screen,
       // so we have to apply the same offset to the mouse coordinates before passing into the shader.
    
    x = (pupilZmq.gaze.norm_pos[0] * ofGetWidth()) - cx; // mouseX - cx
    y = (pupilZmq.gaze.norm_pos[1] * ofGetHeight()) - cy; // mouseY - cy
    
    //float mx = mouseX - cx;
    //float my = mouseY - cy;
    
    
       shader.setUniform1f("mouseRange", 150);
       shader.setUniform2f("mousePos", x, -y);
       shader.setUniformTexture("tex0", fbo1.getTexture(), 0);
    
        // send distance to top of line
        // send lineheight
    
        // send bounds of current word
        // send bounds of current line
    

        ofTranslate(cx, cy);
        ofScale(1,-1);

        plane.draw();
        
    //fbo1.draw(0, 0);
       
      shader.end();
    
    
    ofSetColor(0);
    ofDrawCircle(x, y, 10);
    
    
}




