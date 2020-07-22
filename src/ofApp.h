
//
//  ofxParagraphExample
//
//  Created by Stephen Braitsch on 9/17/15
//  https://github.com/braitsch/ofxParagraph
//

#pragma once
#include "ofMain.h"
#include "Paragraph.hpp"
#include "ofxGui.h"

#include "ofxBiquadFilter.h"

#include <tobii_research.h>
#include <tobii_research_streams.h>
#include <tobii_research_eyetracker.h>
//#include <tobii_research_calibration.h>


#define MSGPACK_DISABLE_LEGACY_NIL
#include <msgpack.hpp>
#include <zmq.hpp>


//#include <azmq/socket.hpp>
//#include <boost/asio.hpp>
//#include <array>

// Remember to escape quotes
const string text1 = R"(Vi var kommet forbi striben af firkantede lodder, flade, bortset fra de gule etplanshuse, der var skudt op på nogle af dem. Og så lå den der pludselig, vejen. Den var ryddet for sne og skar sig gennem alt det hvide i begge retninger, så langt man kunne se. Det så pænt ud. Måske forstærket af alt roderiet bag os, stierne, grusvejene, markerne og byggepladserne, som vi ikke kunne hitte ud af. På den anden side var en mark, hvid og med striber af sort, fuget jord. Den fortsatte udefter og over i en gråhvid dis. Det var et mærkeligt sted, så langt væk, man kunne komme, og med universet og rummet trukket tæt på. Vi spurgte, om vi var der, om det var dét her. Så løftede han armen og pegede ud over marken, som om han ville kommandere os fremad eller sige "dér ovre, lige på den anden side." Det samme sted eller lidt derfra stod en række kæmpestore betonskeletter, uhyggelige, levende væsener og samtidig papirstynde og skrøbelige og så bittesmå, at man kunne løfte dem op i hånden eller vælte dem ét ad gangen med et puf med det yderste af fingrene. De så fine ud, række efter række og en lille smule forskudt i en skrå linje bagud. Nedefra var de oplyst af projektører, der skar dem fri af mørket og trak dem tættere på, så de i nogle feberagtige, svimlende sekunder så ud til at ville lette. Og så gik vi hjem. Imens fortalte han en af sine historier. Til sidst bar han mig. Det er det år, mine forældre er blevet skilt. Det samme er Louis’ forældre eller gået fra hinanden, for de var ikke rigtig gift ligesom mine. Det var hans mor, der gik, eller løb, hele vejen til Sønderjylland efter at være blevet gennembanket i årevis. Kort før den dag på grillbaren var skoleinspektøren kommet midt i en musiktime og havde givet mig et brev fra Louis, hvor han fortalte det hele. Han skrev også, at hans mor var dum.)";




extern float TobiiX;
extern float TobiiY;


class TobiiManager
{
public:
    
    TobiiResearchEyeTracker* eyetracker;
    char* device_address = NULL;
    char device_address_arg[128];

    // TobiiResearchGazeData
    // TobiiResearchHMDGazeData
    //typedef std::function<int (const TobiManager&)> gaze_data_callback;
    
    static void gaze_data_callback(TobiiResearchGazeData* gaze_data, void* user_data) {
        //memcpy(user_data, gaze_data, sizeof(*gaze_data));
        
        //printf("Last received gaze package:\n");
        //printf("System time stamp: %"  PRId64 "\n", gaze_data->system_time_stamp);
        //printf("Device time stamp: %"  PRId64 "\n", gaze_data->device_time_stamp);
        //printf("Left eye 2D gaze point on display area: (%f, %f)\n",
        //gaze_data->left_eye.gaze_point.position_on_display_area.x,
        //gaze_data->left_eye.gaze_point.position_on_display_area.y);
        
        TobiiX = gaze_data->left_eye.gaze_point.position_on_display_area.x;
        TobiiY = gaze_data->left_eye.gaze_point.position_on_display_area.y;
        
        
        
    }
    
    void connect() {
        TobiiResearchStatus status = tobii_research_get_eyetracker("tobii-prp://TPNA1-030109826674", &eyetracker);
        
        if (status != TOBII_RESEARCH_STATUS_OK) {
              printf("ERROR: %d when trying to get eyetracker!\n", status);
              exit(EXIT_FAILURE);
          }
        tobii_research_get_address(eyetracker, &device_address);
        if (status != TOBII_RESEARCH_STATUS_OK) {
              printf("ERROR: %d when trying to get address!\n", status);
              exit(EXIT_FAILURE);
        }
        
        TobiiResearchGazeData gaze_data;
        status = tobii_research_subscribe_to_gaze_data(eyetracker, this->gaze_data_callback, &gaze_data);
        
        if (status != TOBII_RESEARCH_STATUS_OK) {
            printf("ERROR: %d when trying to get address!\n", status);
            exit(EXIT_FAILURE);
        }
        /*
        printf("Left eye 2D gaze point on display area: (%f, %f)\n",
                  gaze_data.left_eye.gaze_point.position_on_display_area.x,
                  gaze_data.left_eye.gaze_point.position_on_display_area.y);
        printf("Right eye 3d gaze origin in user coordinates (%f, %f, %f)\n",
                  gaze_data.right_eye.gaze_origin.position_in_user_coordinates.x,
                  gaze_data.right_eye.gaze_origin.position_in_user_coordinates.y,
                  gaze_data.right_eye.gaze_origin.position_in_user_coordinates.z);
         */
        
        //tobii_research_find_all_eyetrackers();
        // tobii_research_subscribe_to_gaze_data
        // tobii_research_subscribe_to_hmd_gaze_data
    }
    
    void get_data() {
        /*printf("Left eye 2D gaze point on display area: (%f, %f)\n",
        TobiiX,
        TobiiY);*/
    }
    
};


struct Gaze
{
    //public:
        std::string topic;
        float norm_pos [2];
        float eye_center_3d [3];
        float gaze_normal_3d [3];
        float gaze_point_3d [3];
        
        float confidence;
        float timestamp; // cast to time / unsigned long int - seems to be float in the msgpack encoding
    
        msgpack::object base_data; // 'base_data': [<pupil datum>]
        MSGPACK_DEFINE_MAP(topic, norm_pos, eye_center_3d, gaze_normal_3d, gaze_point_3d, confidence, timestamp, base_data);
};

struct Pupil
{
    //public:
        std::string topic;
    
        float norm_pos [2];

        float confidence; // TODO: take data above 0.6
        //float timestamp; // cast to time / unsigned long int - seems to be float in the msgpack encoding
    
        //msgpack::object base_data; // 'base_data': [<pupil datum>]
        MSGPACK_DEFINE_MAP(topic, norm_pos, confidence /*eye_center_3d, gaze_normal_3d, gaze_point_3d, confidence, timestamp, base_data*/);
};







class PupilZmq
{
    
    public:
    /*void threadedFunction()
    {
        while(isThreadRunning())
        {
            
        }
    }*/

    void connect(string addr);
    void receive();
    
    Pupil pupil;
    Gaze gaze;
    
    std::string getApiAddress(string addr, int port) {
        std::stringstream ss;
        ss << "tcp://" << addr << ":" << port;
        return ss.str();
    }
    
    protected:
    zmq::socket_t socket;
    zmq::pollitem_t items[1];
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void draw();
        void update();
    
        vector<Paragraph*> paragraphs;
    
    
    ofxBiquadFilter2f filter;
    
    ofShader shader;
    ofPlanePrimitive plane;
        
    ofFbo fbo1;
    ofTexture tex1;
    
    PupilZmq pupilZmq;
    
    TobiiManager tobii;
    
    /*
    lower left 0.38111690686729993,0.4214027202406092
    lower right 0.685358948107331,0.4051380090533916
    top left 0.36712246520437525,0.6716195969513632
    top right 0.6758969988342477,0.6645489801140543
    */
    
    float LEFT_X = 0.42;
    float RIGHT_X = 0.7;
    
    float TOP_Y = 0.45;
    float BOTTOM_Y = 0.67;
    //float TOP_RIGHT_Y = 0.6645489801140543;
    
    //float BOTTOM_LEFT_X = 0.38111690686729993;
    
    //float BOTTOM_RIGHT_X = 0.685358948107331;
    //float BOTTOM_RIGHT_Y = 0.4051380090533916;
    
    float x;
    float y;
    
    //ofParameter<float> radius;
    //ofParameter<ofColor> color;
    //ofParameter<glm::vec2> magnificationArea;
    ofParameter<bool> bUseEyeTracker;
    //ofxButton ringButton;
    //ofParameter<string> screenSize;
    
    ofParameter<bool> pushTextLeft;
    ofParameter<int> numMagnifyLetters;
    ofParameter<float> letterScale;
    
    //ofParameter<float> magnifyDist;

    ofxPanel gui;
    
};






/* example msgpack data
{"topic":"gaze.3d.1.",
    "norm_pos":[0.171095,0.661531],
    "eye_center_3d":[14.7901,76.424,124.037],
    "gaze_normal_3d":[-0.585709,-0.252092,0.770321],
    "gaze_point_3d":[-549.248,-166.341,865.855],
    "confidence":0.288941,
    "timestamp":314099,
    "base_data":[
        {"circle_3d":{"center":[0.248564,0.340241,3.23776],"normal":[-0.57931,0.163686,-0.798503],"radius":0.0282597},
            "confidence":0.288941,
            "timestamp":314099,
            "diameter_3d":0.0565194,
            "ellipse":{"center":[143.621,161.149],
            "axes":[8.86519,10.9114],
            "angle":-26.0557},
            "location":[143.621,161.149],
            "diameter":10.9114,
            "sphere":{
                    "center":[7.20028,-1.62399,12.8198],
                    "radius":12},
                 "projected_sphere":{
                    "center":[444.225,17.4592],
                    "axes":[1160.71,1160.71],
                    "angle":90},
                 "model_confidence":1,
                 "model_id":141,
                 "model_birth_timestamp":313692,
                 "theta":1.73522,
                 "phi":-2.19843,
                 "norm_pos":[0.748025,0.160685],
                 "topic":"pupil.1",
                 "id":1,
                 "method":"3d c++"}
 ]
 }
*/
