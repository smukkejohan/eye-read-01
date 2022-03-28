

//#define USE_TOBII_TRACKER false

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
#define MSGPACK_TYPE_BOOST_MSGPACK_VARIANT_HPP

#include <msgpack.hpp>
#include <zmq.hpp>

//#include <azmq/socket.hpp>
//#include <boost/asio.hpp>
//#include <array>

// Remember to escape quotes
const string text1 = R"(Vi var kommet forbi striben af firkantede lodder, flade, bortset fra de gule etplanshuse, der var skudt op på nogle af dem. Og så lå den der pludselig, vejen. Den var ryddet for sne og skar sig gennem alt det hvide i begge retninger, så langt man kunne se. Det så pænt ud. Måske forstærket af alt roderiet bag os, stierne, grusvejene, markerne og byggepladserne, som vi ikke kunne hitte ud af. På den anden side var en mark, hvid og med striber af sort, fuget jord. Den fortsatte udefter og over i en gråhvid dis. Det var et mærkeligt sted, så langt væk, man kunne komme, og med universet og rummet trukket tæt på. Vi spurgte, om vi var der, om det var dét her. Så løftede han armen og pegede ud over marken, som om han ville kommandere os fremad eller sige "dér ovre, lige på den anden side." Det samme sted eller lidt derfra stod en række kæmpestore betonskeletter, uhyggelige, levende væsener og samtidig papirstynde og skrøbelige og så bittesmå, at man kunne løfte dem op i hånden eller vælte dem ét ad gangen med et puf med det yderste af fingrene. De så fine ud, række efter række og en lille smule forskudt i en skrå linje bagud. Nedefra var de oplyst af projektører, der skar dem fri af mørket og trak dem tættere på, så de i nogle feberagtige, svimlende sekunder så ud til at ville lette. Og så gik vi hjem. Imens fortalte han en af sine historier. Til sidst bar han mig. Det er det år, mine forældre er blevet skilt. Det samme er Louis forældre eller gået fra hinanden, for de var ikke rigtig gift ligesom mine. Det var hans mor, der gik, eller løb, hele vejen til Sønderjylland efter at være blevet gennembanket i årevis. Kort før den dag på grillbaren var skoleinspektøren kommet midt i en musiktime og havde givet mig et brev fra Louis, hvor han fortalte det hele. Han skrev også, at hans mor var dum.)";

const string text2 = R"(Louis hørte selvfølgelig også med. Han dukkede op en dag, forsvandt og dukkede op en gang til. Og til sidst forsvandt han så helt. Det blev ikke til noget med den flugt, han havde fablet om. Det vidste jeg godt. Det var ikke, fordi han ikke mente det, det var heller ikke, fordi han ikke kunne have klaret turen. Han plejede at sige, “Det gør vi da bare”, og så gjorde han det. Bare ikke denne gang. Når det kom til stykket, kunne han ikke lade sin mor sidde alene tilbage i en flække i Sønderjylland. Det ville have taget livet af hende.
Han kom tilbage det næste forår. Hans mor havde fået arbejde i kantinen på et bomuldsspinderi lidt uden for byen, og hans far, som ikke var hans rigtige far, var taget til Norge for at bygge huse og tjene kassen. På et eller andet tidspunkt ville han så komme rullende op foran køkkenet på bomuldsspinderiet i den nyeste Volvo med fire døre, og invitere Louis’ mor ud på engelsk bøf og en bagt kartoffel og en sang fra de varme lande. Han ville smile til hende, hive sig i skægget og ryste på hovedet og sige, “Gud, hvor har jeg altså savnet dig”. Og Ellen ville sige, “Jamen, altså Erik, det går ikke”, og det ville hun sige en gang til og så en gang til og måske endnu et par gange. Og når de så havde ædt sig gennem et bjerg af bagte kartofler og smør med persille og hvidløg og nogle kilo sejt oksekød, så ville han en aften sende hende et blik hen over bordet, som burde få hende til at springe op fra stolen og spurte hele vejen tilbage til den sønderjyske landsby og den skolelærer, hun traf dernede. Men se om hun gør det. Hun bliver siddende. Og på et eller andet tidspunkt - om nogle timer, nogle dage, uger eller måneder, allerhøjest et par år - kommer hun styrtende ind på Louis’ værelse med sminken opløst af snot og gråd, med løse fortænder og to flækkede øjenbryn.
)";

const string text3 = R"(Ellen passede sit kantine-job og tog i weekenden på diskotek og dansede moderne, som hun kaldte det. Som noget af det første fik hun ordnet brysterne, måske for at komme sig over skolelæreren eller gøre klar til en ny. Fra en gang i maj brunede hun dem på altanen, og lavede man ikke for meget larm i entreen, kunne man være heldig at se dem, inden de forsvandt tilbage i skålene.
Louis havde forandret sig. Da jeg lærte ham at kende, da han begyndte på skolen i tredje klasse, havde han lignet en lysebrun plysbjørn, som nogen havde givet en kogevask og bagefter centrifugeret, rullet, presset og banket ti gange hårdt mod asfalten. Når vi spillede fodbold, roterede han med arme og ben og skræppede til højre og venstre på en dialekt, han kunne have samlet op i et af tv’s børneprogrammer, men med garanti ikke på den kro i Kragelund, han kom fra, “Dér røg du på slangen!”, hvad ingen os andre anede og ikke nænnede at spørge ham om, hvad skulle betyde. I timerne bøjede han sig over det samme krøllede kladdehæfte, hvad enten faget var matematik eller dansk eller religion eller samtidsorientering, og malede løs med blyanten, indtil grafitten stod i skyer omkring ham. Hvad han skrev, er ikke til at sige, for når det ringede ud, mosede han hæfte og blyant ned i bunden af tasken, og som om nogen havde trykket på en knap i ryggen, begyndte hans ben at skælve og armene at sitre, inden rystelserne satte sig i stemmebåndet som en snurren. Det ændrede sig langsomt, han fandt sig til rette, men ikke helt. Det var, som om hans hoved var ét mylder af elektriske faresignaler, som han reagerede på ved enten at bide sig hårdere fast eller få røven med sig. Men efterhånden fandt han alligevel en form, som mere og mere blev til den Louis, verden fik at se: Den charmerende, smilende, blinkende bamse.
)";

const string text4 = R"(Madsen kunne det hele, han kunne også være en idiot og bagefter sige undskyld. Han sagde noget om min far. At han var arrogant eller opblæst eller uforskammet eller noget andet, som ikke engang en skolelærer med overarme som en sværvægtsbokser kan slippe af sted med. Og så kom han hen og sagde undskyld, og bagefter har jeg sikkert skrevet en dansk stil med hundrede sytten følsomme beskeder mellem linjerne, men først og fremmest den, at det var i orden, herregud.
For os tilhørte han den forskelsløse voksenkategori, men han må have været en ung mand. I midten af trediverne, måske lidt yngre. Han boede på landet i en nedlagt landbrugsejendom. Der var også en kone og en datter. Vi var der engang i det tidlige efterår, hele klassen. Vi slog telte op på en mark, og så gik natten med bål, musik fra kassettebåndoptageren og løben frem og tilbage og ind og ud af teltene i en rus af anelser og spændt venten og total forvirring. Næste formiddag lå vi i græsset under et pæretræ. Der var franskbrød og marmelade og juice. På et tidspunkt fortæller han, at han har fået kræft, og at det sidder i maven. Bare sådan, og ingen sagde et ord, så var det både sagt og glemt. Bagefter pakkede vi teltene sammen og cyklede ud til et produktionskollektiv med eget mejeri og humant komposteringsanlæg. Bjerge af menneskelort. Fyren, der viste rundt, var mere begejstret, end vi var. Først kiggede han fra den ene til den anden, som om han aldrig havde set noget mere håbløst. Så spurgte han, om lort måske var noget at være flov over. Og om vi måske ikke også selv gik på lokum, og om drengene troede, at pigernes lugtede anderledes, og om nogen ville fortælle ham, at de aldrig havde fået noget på fingrene og bagefter stukket dem op under næsen. Uden at sige mere lod han hele underarmen forsvinde ned i bunken og rakte bagefter hånden frem mod os.
)";



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
              throw "ERROR when trying to get eyetracker!";
              //exit(EXIT_FAILURE);
          }
        tobii_research_get_address(eyetracker, &device_address);
        if (status != TOBII_RESEARCH_STATUS_OK) {
              printf("ERROR: %d when trying to get address!\n", status);
              throw "ERROR when trying to get eyetracker!";
              //exit(EXIT_FAILURE);
        }
        
        TobiiResearchGazeData gaze_data;
        status = tobii_research_subscribe_to_gaze_data(eyetracker, this->gaze_data_callback, &gaze_data);
        
        if (status != TOBII_RESEARCH_STATUS_OK) {
            printf("ERROR: %d when trying to get address!\n", status);
            //exit(EXIT_FAILURE);
            throw "ERROR when trying to get eyetracker!";
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
        void exit();
    
    
    void loadSettings(string name);
    
        void keyPressed(int key);
    void keyReleased(int key);

        // audio stuff
        void audioOut(ofSoundBuffer & buffer);
        ofSoundStream soundStream;
        float pan;
        int sampleRate;
        bool bNoise;
        float volume;
        
        vector <float> lAudio;
        vector <float> rAudio;
        
        // synth
        float     targetFrequency;
        float     phase;
        float     phaseAdder;
        float     phaseAdderTarget;
        
        void magScaleChanged(float & value) {
            //paragraphs[0]->setFont("Helvetica", pFontSize, value);
        }
        
        vector<Paragraph*> paragraphs;
        ofxBiquadFilter2f filter;
        
        ofShader shader;
        ofPlanePrimitive plane;
        ofFbo fbo1;
        ofTexture tex1;
        PupilZmq pupilZmq;
        TobiiManager tobii;
        
        //file logging
        ofFile file;
        /*
        lower left 0.38111690686729993,0.4214027202406092
        lower right 0.685358948107331,0.4051380090533916
        top left 0.36712246520437525,0.6716195969513632
        top right 0.6758969988342477,0.6645489801140543
        */
        /*float LEFT_X = 0.42;
        float RIGHT_X = 0.7;
        float TOP_Y = 0.45;
        float BOTTOM_Y = 0.67;*/
        //float TOP_RIGHT_Y = 0.6645489801140543;
        //float BOTTOM_LEFT_X = 0.38111690686729993;
        //float BOTTOM_RIGHT_X = 0.685358948107331;
        //float BOTTOM_RIGHT_Y = 0.4051380090533916;
        
        unsigned int long lineChangeTimeNext = 0;
        unsigned int long lineChangeTimePrevious = 0;

        //bool targetNewLine = true;
        float yTarget = 0;
        float rawx;
        float rawy;
        float x;
        float y;
        
        //ofParameter<float> radius;
        //ofParameter<ofColor> color;
        //ofParameter<glm::vec2> magnificationArea;
        ofParameter<bool> bUseEyeTracker;
        //ofxButton ringButton;
        //ofParameter<string> screenSize;
        ofParameter<bool> pushText;
        ofParameter<bool> magnifyWholeWords;
        ofParameter<int> numLettersLeft;
        ofParameter<int> numLettersRight;
        ofParameter<float> magnifyScale;
        //ofParameter<float> magnifyDist;
        ofParameter<double> filterFc;
        ofParameter<double> filterQ;
        ofParameter<int> lineChangeNextDwellMs;
        ofParameter<int> lineChangePreviousDwellMs;
        
        bool drawGui = false;
        ofxPanel gui;
        ofParameter<bool> recordData;
        ofParameter<bool> showCursor;
        ofParameter<bool> showAttractPoint;
        
        ofParameter<int> freezeLastWordDwellTime;
        ofParameter<int> lineTransitionDwellTime;
        
        ofParameter<int> mode;
        
        ofParameter<double> magXFilterFc;
        ofParameter<double> magXFilterQ;
        
        ofParameterGroup all_params;
        ofParameterGroup zoom_mode_params;
        ofParameterGroup hint_mode_params;
        ofParameterGroup hidden_params;
    
        ofParameter<string> textParam;

        const int ZOOM_READING_MODE = 0;
        const int RETURN_HINT_MODE = 1;
        
        bool lookAway = false;
        
        bool hintActive = false;
        unsigned int long hintActiveTime = 0;

        unsigned int long lookBackOrAwayTime = 0;
        ofParameter<int> hintDurationMs;
        ofParameter<int> hintLookAwayThresholdMs;
        
        ofParameter<int> hintLookAwayDeadzonePadding;
        
        ofParameter<int> hintExtendBack;
        ofParameter<int> hintExtendForward;
        
        ofParameter<int> hintAudioDurationMs;

        
        ofParameter<float> audioHintAmp;
        ofParameter<float> audioHintTargetFreq;

        ofParameter<bool> audioHintNoise;
        ofParameter<bool> audioHintVariableFreq;
        
        ofVec2f lastLookAtPosition;
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
