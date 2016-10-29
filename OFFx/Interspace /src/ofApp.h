#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "maxiMFCC.h"

#define OSC_IN_PORT 6666


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        bool removeTest( ofPoint & a);

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ |  maximilian stuff |
		void audioRequested(float * input, int bufferSize, int nChannels);
		void audioReceived(float * input, int bufferSize, int nChannels);

		float 	* lAudioOut;
		float   * rAudioOut;
		float * lAudioIn;
		float * rAudioIn;
		int		initialBufferSize;
		int		sampleRate;

		double wave, sample, outputs[2], ifftVal;
		maxiMix mymix;
		maxiOsc osc;

		ofxMaxiFFTOctaveAnalyzer oct;
		int nAverages;
		float *ifftOutput;
		int ifftSize;
		float peakFreq = 0;
		float centroid = 0;
		float RMS = 0;
		ofxMaxiIFFT ifft;
		ofxMaxiFFT mfft;
		int fftSize;
		int bins, dataSize;
		maxiMFCC mfcc;
		double *mfccs;
		maxiSample samp;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | buffer stuff |
		vector< vector<float> > dataBuff;
		vector<float> d0;
		vector<float> mB;
		int dBuffSize;
		int nBands;
		int rot;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | osc stuff |
        float p01, p02, p03, p04;
        ofxOscReceiver receiver;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | gui stuff |
		bool showGui;

		ofxPanel gui;

		ofxGuiGroup mixerGroup;
		ofxGuiGroup sc1Group;
		ofxGuiGroup sc2Group;
		ofxGuiGroup sc3Group;
		ofxGuiGroup sc4Group;
		ofxGuiGroup sc5Group;
		ofxGuiGroup sc6Group;
		ofxGuiGroup envGroup;
		ofxFloatSlider st01, dur01, vel01;
		ofxFloatSlider st02, dur02, vel02;
		ofxFloatSlider radJ, phaseJ;
		ofxFloatSlider rotX3, rotY3;
		ofxFloatSlider extI;
		ofxFloatSlider st03, dur03, vel03;
		ofxFloatSlider velParts, frecParts;
		ofxFloatSlider f01, f02, f03, f04;
		ofxToggle dspEnabled;
		ofxToggle scene1, scene2, scene3, scene4, scene5;
		ofxFloatSlider scFade1, scFade2, scFade3, scFade4, scFade5, scFade6, scFade7;
		ofxToggle scene6, scene7;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | drawing stuff |
		vector<ofPoint> points;
		vector<ofPoint> vertices;
        int h,w, t;
        float tt;
		ofMesh mesh;
		ofPolyline line;
		ofSpherePrimitive sphere;
		ofPoint center;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | sc1 |
        vector<ofTexture> seq_01;
        ofFbo fbo_01;
        int index_01;

        int fNo_01;
        int n_imgs_01;
        ofDirectory dir;
        int tt00, tt1, tt01, tt03;
        int durat02, init02, veloc02;
        int durat01, init01, veloc01;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | sc2 |
        ofFbo fbo_02;
        int index_02;
        vector<ofPoint> circCents;
        vector<int> radius;
        float a, r, aDiff, rDiff;
        int nCents;
        float au1, au2;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | sc3 |
		ofFbo fbo_03;
		ofMesh mesh_03;
        vector<ofTexture> seq_03;
        ofVboMesh mMesh;
        int index_03;
        int n_imgs_03;
        int durat03, init03, veloc03;
        float rX3, rY3;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | sc4 |
        vector<ofPoint> noWave;
        ofFbo fbo_04;
        int nPW;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | sc5 |
		ofFbo fbo_05;
		vector<ofPoint> parts;
		vector<ofPoint> parts2;
		vector<int> vels;
		int nParts, nParts2;

		// .... .... .... ____ ____ ____ #### ------ ------ ------ ------ ------ ------ | sc6 | dots
        ofFbo fbo_06;
		vector<ofPoint> cells;
		int nDots;

};
