#include "ofApp.h"

/*
dSpace immersion
1. ignicion && despegue
2. scape velocity
3. 65daysofstatic
4. calm && bitacora - intermission
5. infinite and beyond
*/



// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ..............
//---------- ---------- ----------   ----------   ---------- ---------- ---------- | >> SETUP   //
// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ··············
void ofApp::setup() {
	h = ofGetHeight();
	w = ofGetWidth();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);

	// .. __ .. __  .. __ .. __ .. __ .. __ .. __ .. __ .. __ .. __ --- --- --- OSC
	receiver.setup(OSC_IN_PORT);
	cout << "listening for osc messages on port " << OSC_IN_PORT << "\n";

	// .. __ .. __  .. __ .. __ .. __ .. __ .. __ .. __ .. __ .. __ --- --- --- AUDIO
	sampleRate = 44100;
	initialBufferSize = 512;
	lAudioOut = new float[initialBufferSize];
	rAudioOut = new float[initialBufferSize];
	lAudioIn = new float[initialBufferSize];
	rAudioIn = new float[initialBufferSize];
	memset(lAudioOut, 0, initialBufferSize * sizeof(float));
	memset(rAudioOut, 0, initialBufferSize * sizeof(float));
	memset(lAudioIn, 0, initialBufferSize * sizeof(float));
	memset(rAudioIn, 0, initialBufferSize * sizeof(float));

	fftSize = 1024;
	mfft.setup(fftSize, 512, 256);
	ifft.setup(fftSize, 512, 256);
	nAverages = 12;
	oct.setup(sampleRate, fftSize / 2, nAverages);
	mfccs = (double*)malloc(sizeof(double) * 13);
	mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
	ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
	ofSoundStreamSetup(2, 2, this, sampleRate, initialBufferSize, 4); // last stop

	nBands = 104;
	dBuffSize = 32;
	d0.resize(nBands);
	mB.resize(nBands);
	dataBuff.resize(dBuffSize);
	for (int n = 0; n < dBuffSize; n++) { dataBuff[n].resize(nBands); }

	// .. __ .. __  .. __ .. __ .. __ .. __ .. __ .. __ .. __ .. __ --- --- --- GUI
	showGui = true;
	gui.setup();
	envGroup.setHeaderBackgroundColor(ofColor::white);
	envGroup.setBorderColor(ofColor::white);
	envGroup.setTextColor(ofColor::black);
    envGroup.setup("enTORNO");
	envGroup.add(dspEnabled.setup("dSP", true));
	envGroup.add(scene1.setup("scene1", false));
	envGroup.add(scFade1.setup("sc1FADE", 0.1, 0, 1.0));
	envGroup.add(scene2.setup("scene2", false));
	envGroup.add(scFade2.setup("sc2FADE", 0.1, 0, 1.0));
	envGroup.add(scene3.setup("scene3", false));
	envGroup.add(scFade3.setup("sc3FADE", 0.1, 0, 1.0));
	//envGroup.add(scene4.setup("scene4", false));
	envGroup.add(scene5.setup("scene5", false));
	envGroup.add(scFade5.setup("sc5FADE", 0.1, 0, 1.0));
	//envGroup.add(scene6.setup("scene6", false));
	//envGroup.add(scene7.setup("scene7", false));
    gui.add(&envGroup);

	sc1Group.setHeaderBackgroundColor(ofColor::gray);
	sc1Group.setBorderColor(ofColor::gray);
    sc1Group.setup("sCN//01");
	sc1Group.add(st01.setup("start_01", 0.1, 0, 1.0));
	sc1Group.add(dur01.setup("duration_01", 0.25, 0, 1.0));
	sc1Group.add(vel01.setup("veloc_01", 0.5, 0, 1.0));
	sc1Group.add(st02.setup("start_02", 0.5, 0, 1.0));
	sc1Group.add(dur02.setup("duration_02", 0.5, 0, 1.0));
	sc1Group.add(vel02.setup("veloc_02", 0.5, 0, 1.0));
	gui.add(&sc1Group);

	sc2Group.setHeaderBackgroundColor(ofColor::green);
	sc2Group.setBorderColor(ofColor::gray);
    sc2Group.setup("sCN//02");
    sc2Group.add(radJ.setup("radiusJUMP", 1, 1, 10));
	sc2Group.add(phaseJ.setup("phaseDIFF", 1, 1, 180));
    //sc2Group.add(rotX3.setup("rotateX", 1, 1, 10));
	//sc2Group.add(rotY3.setup("rotateY", 1, 1, 180));
	gui.add(&sc2Group);

	sc3Group.setHeaderBackgroundColor(ofColor::red);
	sc3Group.setBorderColor(ofColor::gray);
    sc3Group.setup("sCN//03");
	sc3Group.add(st03.setup("start_03", 0.5, 0, 1.0));
	sc3Group.add(dur03.setup("duration_03", 0.5, 0, 1.0));
	sc3Group.add(vel03.setup("veloc_03", 0.5, 0, 1.0));
	gui.add(&sc3Group);

	sc5Group.setHeaderBackgroundColor(ofColor::blue);
	sc5Group.setBorderColor(ofColor::gray);
    sc5Group.setup("sCN//05");
    sc5Group.add(velParts.setup("particlesVEL", 30, 1, 300));
    sc5Group.add(frecParts.setup("partPERIOD", 30, 1, 120));
	gui.add(&sc5Group);

    sc6Group.setHeaderBackgroundColor(ofColor::orange);
	sc6Group.setBorderColor(ofColor::gray);
    sc6Group.setup("sCN//06");
    sc6Group.add(f01.setup("param_6.1", 1, 1, 10));
	sc6Group.add(f02.setup("param_6.2", 1, 1, 180));
	gui.add(&sc6Group);
    //sc2Group.add(rotX3.setup("rotateX", 1, 1, 10));


	// .. __ .. __  .. __ .. __ .. __ .. __ .. __ .. __ .. __ .. __ --- --- --- DRAW
	points.resize(1);
	vertices.resize(1);

	sphere.set(250, 200);
	vertices = sphere.getMesh().getVertices();
	sphere.mapTexCoords(0, h, w, 0);
	sphere.rotate(90, 0, 1, 0);

	center = ofPoint(0,0,0);
    tt00 = ofGetElapsedTimeMillis();
    tt01 = ofGetElapsedTimeMillis();
    index_02 = 0;
    index_01 = 0;


    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc01
    fbo_01.allocate(w, h, GL_RGBA);
    fbo_01.begin(); ofClear(0, 0); fbo_01.end();

   	//n_imgs_01 = dir.listDir("bb_ss02");
   	n_imgs_01 = dir.listDir("imgs_casini");
    dir.sort();
	seq_01.resize(n_imgs_01);
	for (int i = 0; i < n_imgs_01; i++) {
		string fn = dir.getPath(i);
		ofLoadImage(seq_01[i], fn);
		cout << "Reading_IMGS01: " << i << "/" << n_imgs_01 << "\n";
	}

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc02
    fbo_02.allocate(w, h, GL_RGBA);
    fbo_02.begin(); ofClear(0, 0); fbo_02.end();
    circCents.resize(1);
    radius.resize(1);
    nCents = 200;
    for (int np = 0; np <nCents; np++){
            ofPoint p(0, 0 , 0 );
            circCents.push_back(p);
            radius.push_back(0);
    }

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc03
    fbo_03.allocate(w, h, GL_RGBA);
    fbo_03.begin(); ofClear(0, 0); fbo_03.end();

    n_imgs_03 = dir.listDir("imgs_spaceStation");
    dir.sort();
	seq_03.resize(n_imgs_03);
	for (int i = 0; i < n_imgs_03; i++) {
		string fn = dir.getPath(i);
		ofLoadImage(seq_03[i], fn);
		cout << "Reading_IMGS03: " << i << "/" << n_imgs_03 << "\n";
	}
    index_03 = 0;

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc04
    fbo_04.allocate(w, h, GL_RGBA);
    fbo_04.begin(); ofClear(0, 0); fbo_04.end();
    nPW = 300;
    noWave.resize(1);
    for (int i=0; i<nPW; i++){
        ofPoint p(w/2, h/2, 100 - i*2);
        noWave.push_back(p);
    }

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc05
    fbo_05.allocate(w, h, GL_RGBA);
    fbo_05.begin(); ofClear(0, 0); fbo_05.end();
    parts.resize(1);
    parts2.resize(1);
    vels.resize(1);

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc06
    fbo_06.allocate(w, h, GL_RGBA);
    fbo_06.begin(); ofClear(0, 0); fbo_06.end();
    nDots = 300;
    cells.resize(1);
    for (int i=0; i<nDots; i++){
        float a = ofRandom(0, TWO_PI);
        float r = ofMap(i, nDots, 0, 1, h/2);
        ofPoint po(w/2 + r*cos(a), h/2 + r*sin(a), 0);
        cells.push_back(po);
        //cells[i] = new Cell( hw + r*cos(a), hh + r*sin(a), i, r, a, b );
    // keep on going
    }

}



// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ..............
//---------- ---------- ----------   ----------   ---------- ---------- ---------- | >> UPDATE  //
// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ··············
void ofApp::update(){
    // .. __ .. __  .. __ .. __ .. __ .. __ .. __ .. __ .. __ .. __ --- --- --- OSC
    t = ofGetElapsedTimeMillis();
	tt = ofGetElapsedTimef();

	while (receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage(m);
		// --- mixer
		if (m.getAddress() == "/hspace/dspEnabled") {
			dspEnabled = m.getArgAsBool(0);
			cout << "dspEnabled: " << dspEnabled<< "\n";
		}
		if (m.getAddress() == "/hspace/scene1") {
			scene1 = m.getArgAsBool(0);
			cout << "scene1: " << scene1 << "\n";
		}
		else if (m.getAddress() == "/hspace/scene2") {
			scene2 = m.getArgAsBool(0);
			cout << "scene2: " << scene2 << "\n";
		}
		else if (m.getAddress() == "/hspace/scene3") {
			scene3 = m.getArgAsBool(0);
			cout << "scene3: " << scene3 << "\n";
		}
		else if (m.getAddress() == "/hspace/scene5") {
			scene5 = m.getArgAsBool(0);
			cout << "scene5: " << scene5 << "\n";
		}
		else if (m.getAddress() == "/hspace/scFade1") {
			scFade1= m.getArgAsFloat(0);
			cout << "scFade1: " << scFade1 << "\n";
		}
		else if (m.getAddress() == "/hspace/scFade2") {
			scFade2= m.getArgAsFloat(0);
			cout << "scFade2: " << scFade2 << "\n";
		}
		else if (m.getAddress() == "/hspace/scFade3") {
			scFade3= m.getArgAsFloat(0);
			cout << "scFade3: " << scFade3 << "\n";
		}
		else if (m.getAddress() == "/hspace/scFade5") {
			scFade5= m.getArgAsFloat(0);
			cout << "scFade5: " << scFade5 << "\n";
		}

        // ---- scene 1
		else if (m.getAddress() == "/hspace/sc1/st01") {
			st01= m.getArgAsFloat(0);
			cout << "st01: " << st01 << "\n";
		}
		else if (m.getAddress() == "/hspace/sc1/st02") {
			st02= m.getArgAsFloat(0);
			cout << "st02: " << st02 << "\n";
		}
		else if (m.getAddress() == "/hspace/sc1/dur01") {
			dur01= m.getArgAsFloat(0);
			cout << "dur01: " << dur01 << "\n";
		}
		else if (m.getAddress() == "/hspace/sc1/dur02") {
			dur02= m.getArgAsFloat(0);
			cout << "dur02: " << dur02 << "\n";
		}
		else if (m.getAddress() == "/hspace/sc1/vel01") {
			vel01= m.getArgAsFloat(0);
			cout << "vel01: " << vel01 << "\n";
		}
		else if (m.getAddress() == "/hspace/sc1/vel02") {
			vel02= m.getArgAsFloat(0);
			cout << "vel02: " << vel02 << "\n";
		}

        // ---- scene 2
		else if (m.getAddress() == "/hspace/sc2/radJ") {
			radJ= m.getArgAsFloat(0);
			cout << "radJ: " << radJ << "\n";
		}
		else if (m.getAddress() == "/hspace/sc2/phaseJ") {
			phaseJ= m.getArgAsFloat(0);
			cout << "phaseJ: " << phaseJ << "\n";
		}

        // ---- scene 3
		else if (m.getAddress() == "/hspace/sc3/st03") {
			st03= m.getArgAsFloat(0);
			cout << "st03: " << st03 << "\n";
		}
		else if (m.getAddress() == "/hspace/sc3/dur03") {
			dur03= m.getArgAsFloat(0);
			cout << "dur03: " << dur03 << "\n";
		}
		else if (m.getAddress() == "/hspace/sc3/vel03") {
			vel03= m.getArgAsFloat(0);
			cout << "vel03: " << vel03 << "\n";
		}

        // ---- scene 5
		else if (m.getAddress() == "/hspace/sc5/velParts") {
			velParts= m.getArgAsFloat(0);
			cout << "velParts: " << velParts << "\n";
		}
		else if (m.getAddress() == "/hspace/sc5/frecParts") {
			frecParts= m.getArgAsFloat(0);
			cout << "frecParts: " << frecParts<< "\n";
		}
	}



	// .. __ .. __  .. __ .. __ .. __ .. __ .. __ .. __ .. __ .. __ --- --- --- BUFFER
	if (dspEnabled) {
		for (int b = 0; b < nBands; b++) {	if (oct.averages[b] > 0) { d0[b] = oct.averages[b]; } 	}
		for (int n = dBuffSize - 1; n > 0; n--) { dataBuff[n] = dataBuff[n - 1]; }
		dataBuff[0] = d0;
		for (int b = 0; b < nBands; b++) {
			mB[b] = 0;
			for (int n = 0; n < dBuffSize; n++) { mB[b] += dataBuff[n][b]; }
			mB[b] /= dBuffSize;
		}
	}

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc01
    if (scene1){
        //cout << mB[24] << endl;
        if (mB[24]>6){
            st01 = ofRandom(0,1);
        }
        if (mB[48]>5){
            st02 = ofRandom(0,1);
        }

        init01  = int( ofMap(st01, 0, 1, 1, n_imgs_01) );
        durat01 = int( ofMap(dur01, 0, 1, 2, 100) );
        veloc01 = int( ofMap(vel01, 0, 1, 90, 2) );

        init02  = int( ofMap(st02, 0, 1, 1, n_imgs_01) );
        durat02 = int( ofMap(dur02, 0, 1, 2, 100) );
        veloc02 = int( ofMap(vel02, 0, 1, 30, 0.1) );

        if (index_01<n_imgs_01-durat01 && index_01<init01+durat01) {
            tt1 = ofGetElapsedTimeMillis();
            if ((tt1 - tt00) > veloc01) {
                index_01++;
                fbo_01.begin();
                ofEnableAlphaBlending();
                seq_01[index_01].draw(0, h/2 - h/6, w/3, h/3 );
                seq_01[index_01].draw(2*w/3, h/2 - h/6, w/3, h/3 );
                ofDisableAlphaBlending();
                fbo_01.end();
                tt00 = ofGetElapsedTimeMillis();
            }
        } else {
            index_01 = init01;
        }

        if (index_02<n_imgs_01-durat02 && index_02<init02+durat02) {
            tt1 = ofGetElapsedTimeMillis();
            if ((tt1 - tt01) > veloc02) {
                index_02++;
                fbo_01.begin();
                seq_01[index_02].draw(w/3, h/2 - h/6, w/3, h/3 );
                fbo_01.end();
                tt01 = ofGetElapsedTimeMillis();
            }
        } else {
            index_02 = init02;
        }
    } // end scene 01


    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc02
    if (scene2){
        if (mB[24]>6){
            phaseJ = ofRandom(1,100);
        }
        a=0;
        r=0;
        rDiff = ofLerp(rDiff, radJ, 0.1);
        aDiff = ofLerp(aDiff, phaseJ, 0.1);
        for (int i=0; i<circCents.size(); i++){
            a+=aDiff;
            if (a>360){
                a = a-360;
                r += rDiff;
            }
            //ofPoint p(r*cos(a), r*sin(a), ofMap(mB[i%nBands], 0, 2, -40, 40));
            ofPoint p(r*cos(a), r*sin(a), 20*sin(r*r));
            circCents[i] = p;
            radius[i] = r;
        }

        fbo_02.begin();
        ofClear(0, 0);
        ofSetColor(255, 255);
        ofNoFill();
        ofPushMatrix();
        ofTranslate(w/2, h/2,0);
        ofRotate(int(t/33)%360, 1,0,0);
        ofRotate(int(t/100)%360, 0,0,1);
        for(int i=0; i<circCents.size(); i++){
            ofSetLineWidth(1);
            ofSetColor(255, 184);
            ofCircle(circCents[i], 100 - ofClamp( 5*mB[int(radius[i])%104], 1, 50 ));
            ofSetColor(255, 255);
            ofCircle(circCents[i]-ofVec3f(0,0,66), ofClamp( 10*mB[int(radius[i])%104], 1, 100 ));
            ofSetLineWidth(2);
            ofSetColor(0,0, 255, 192);
            ofLine(circCents[i].x, circCents[i].y, circCents[i].z, circCents[i].x, circCents[i].y, circCents[i].z+ofClamp( 10*mB[int(radius[i])%104], 1, 200 ) );
            ofSetLineWidth(2);
            ofSetColor(255, 137);
            ofLine(circCents[i].x, circCents[i].y, circCents[i].z-100, circCents[i].x, circCents[i].y, circCents[i].z-ofClamp( 10*mB[int(radius[i])%104], 1, 200 ) );
        }
        //for (auto c: circCents ){ ofCircle(c, 10); }
        ofPopMatrix();
        fbo_02.end();
    }

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc03
    if (scene3){
        // -----
        if (mB[24]>6){
            st03 = ofRandom(0,1);
        }
        init03  = int( ofMap(st03, 0, 1, 1, n_imgs_01) );
        durat03 = int( ofMap(dur03, 0, 1, 2, 500) );
        veloc03 = int( ofMap(vel03, 0, 1, 120, 2) );

        if (index_03<n_imgs_03-durat03 && index_03<init03+durat03) {
            tt1 = ofGetElapsedTimeMillis();
            if (abs(tt1 - tt00) > veloc03) {
                index_03++;
                fbo_03.begin();
                ofEnableAlphaBlending();
                seq_03[index_03].draw(0, 0, w, h);
                ofDisableAlphaBlending();
                fbo_03.end();
                tt00 = ofGetElapsedTimeMillis();
            }
        } else {
            index_03 = init03;
        }
	}

	// ... ... ... --- --- ... ... ... --- --- ... ... --- sc04
    if (scene4){
        for (int i=0; i<nPW; i++){
            float ii=i;
            float nPWnPW = nPW;
            float nBB = nBands;
            noWave[i].y = (h/2) -20 * mB[int(nBB*(ii/nPWnPW))];
            noWave[i].x = (w/2) +20*mB[int(nBB*(ii/nPWnPW))]*sin(4*PI*(ii/nPWnPW));
        }
        fbo_04.begin();
        ofClear(0, 0);
        ofSetLineWidth(1);
        ofSetColor(255, 255);
        ofNoFill();


        for (int i=0; i<nPW; i++){
            ofRect(noWave[i], 5, 5);
        }
        fbo_04.end();
    }

    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc05
    if (scene5){
        if(ofGetFrameNum()%int(frecParts)==0 && parts.size()<2000){
            ofPoint p(ofRandom(w), 0, -3000);
            parts.push_back(p);
            ofPoint p2(ofRandom(w), h, -3000);
            parts2.push_back(p2);
        }
        for (int i=0; i<parts.size(); i++){
            parts[i].z+=velParts;
            parts2[i].z+=velParts;
            if(parts[i].z>300){
                parts.erase(parts.begin()+i);
            }
            if(parts2[i].z>300){
                parts2.erase(parts2.begin()+i);
            }
        }
        //ofRemove(parts, removeTest);
        fbo_05.begin();
        ofClear(0, 0);
        ofSetLineWidth(3);
        ofSetColor(255, 255);
        ofNoFill();
        for (int i=0; i<parts.size(); i++){
            ofPoint di(0,0,-50*mB[i%nBands]);
            ofLine(parts[i], parts[i]+di);
        }
        for (int i=0; i<parts2.size(); i++){
            ofPoint di(0,0,-50*mB[i%nBands]);
            ofLine(parts2[i], parts2[i]+di);
        }
        fbo_05.end();
    }


    // ... ... ... --- --- ... ... ... --- --- ... ... --- sc06
    if (scene6){
        fbo_06.begin();
        for (int i=0; i<nDots;i++){
            ofClear(0, 0);
            ofSetLineWidth(1);
            ofSetColor(255, 255);
            ofLine(cells[i].x, cells[i].y, cells[i].x+20, cells[i].y+20);
        }
        fbo_06.end();
    }
}

bool ofApp::removeTest( ofPoint & a){
    return a.z > 300;
}


// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ..............
//---------- ---------- ----------   ----------   ---------- ---------- ---------- | >> DRAW     //
// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ··············
void ofApp::draw(){
    //ofBackground(ofColor::black);
    //ofEnableAlphaBlending();
    if (scene1){
        ofSetColor(255, 255*scFade1);
        fbo_01.draw(0, 0, w, h);
    }

    if (scene2){
        ofSetColor(255, 255*scFade2);
        fbo_02.draw(0,0);
    }

    if (scene3){
        ofSetColor(255, 255*scFade3);
       	fbo_03.draw(0,0);
    }

    if (scene4){
        ofSetColor(255, 255*scFade4);
        fbo_04.draw(0,0);
    }
    if (scene5){
        ofSetColor(255, 255*scFade5);
        fbo_05.draw(0,0);
    }
    if (scene6){
        ofSetColor(255, 255*scFade6);
        fbo_06.draw(0,0);
    }
    //ofDisableAlphaBlending();

/*
/*
    ofPushMatrix();
    ofTranslate(w/2, h/2, 0);
    for (int i=1; i<4; i++){
        ofSetColor(50*i, 255);
        center.z =  -500 + 100 * mB[i % (nBands / 2)];
        ofCircle(center, 100 );
    }
    ofPopMatrix();
*/

    // --- --- --- gui
    if (showGui) {
		gui.draw();
	}
}



// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ........··......
//---------- ---------- ----------   ----------   ---------- ---------- ---------- | >> FUNCTIONS  //
// ---------- ---------- ----------   ----------   ---------- ---------- -------- - ·············..·
void ofApp::keyPressed(int key){
	if (key == 'h') {
		showGui = !showGui;
	}
	if (key == 'f') {
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {
	for (int i = 0; i < bufferSize; i++) {
		wave = lAudioIn[i];
		if (mfft.process(wave)) {
			mfft.magsToDB();
			oct.calculate(mfft.magnitudesDB);

			float sum = 0;
			float maxFreq = 0;
			int maxBin = 0;
			for (int i = 0; i < fftSize / 2; i++) {
				sum += mfft.magnitudes[i];
				if (mfft.magnitudes[i] > maxFreq) {
					maxFreq = mfft.magnitudes[i];
					maxBin = i;
				}
			}
			centroid = sum / (fftSize / 2);
			peakFreq = (float)maxBin / fftSize * 44100;

			mfcc.mfcc(mfft.magnitudes, mfccs);
		}
		lAudioOut[i] = 0;
		rAudioOut[i] = 0;
	}
}

//--------------------------------------------------------------
void ofApp::audioReceived(float * input, int bufferSize, int nChannels) {
	/* You can just grab this input and stick it in a double, then use it above to create output*/
	float sum = 0;
	for (int i = 0; i < bufferSize; i++) {
		/* you can also grab the data out of the arrays*/
		lAudioIn[i] = input[i * 2];
		rAudioIn[i] = input[i * 2 + 1];

		sum += input[i * 2] * input[i * 2];
	}
	RMS = sqrt(sum);
}
