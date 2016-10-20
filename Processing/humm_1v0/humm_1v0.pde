/*
logDEC's humming
----------------
generative // visuals
one to many // connection
v1.0, july25, 2016
*/



// *_.-·***--·***·*·-._* ·· *_.-·***--·***·*·-._* libs + vars
import processing.sound.*;

FFT fft;
AudioIn in;
int nc = 3000;      // number of cells
float bd = 10;       // base line length
float sp = 0.01;     // rotation speed step
float sl = .98;      // slow down rate
Cell[] cells = new Cell[nc];
int mX, mY;
int dY=0, dX=0;
float auX=0, auY=0;

int bands2 = 256;
int bands = bands2/2;
int LEN_BUFFER_0 = 5;
int LEN_BUFFER_1 =100;

float[] spectra = new float[bands];
float[] past_spectra = new float[bands];
float[][] buff_0 = new float[LEN_BUFFER_0][bands];
float[][] buff_1 = new float[LEN_BUFFER_1][bands];
int[] closest = new int[nc];
float[] means = new float[bands];
float[] mBuff0 = new float[bands];
float[] mBuff1 = new float[bands];

int s,c ;
float sum, mean, sector;
PVector n;
int thrsh02, factor, slider3, offset;
float thrsh01;
boolean mode=false, auto=false, debug=false;
int hw, hh;
int show = 0;
int st = 1;



// *_.-·***--·***·*·-._* ·· *_.-·***--·***·*·-._*  setup
void setup() {
  size(1280, 1200, P3D);
  background(0,0,15);
  frameRate(30);
  noFill();
  strokeCap(ROUND);
  hw = width/2;
  hh = height/2;
  dX=hw;
  dY=hh;
  //init fft & audio
  fft = new FFT(this, bands2);
  in = new AudioIn(this, 0);
  in.start();
  fft.input(in);
  s = width/bands;
  sum=0; c=0; 
  sector = TWO_PI/bands;
  // stuff
  initBuffs();
  initCells();
}      



// *_.-·***--·***·*·-._* ·· *_.-·***--·***·*·-._*  draw
void draw() { 
  background(0,0,15);
  updateBuffs();
  if (debug){
    drawRing();
  }
  //perspective handling
  pushMatrix();
  //rotate
  translate(hw,hh,0);
  rotate(-frameCount%7200*TWO_PI/7200);
  translate(-hw,-hh,0);
  // visusal env
  switch (show){
    case 5: //shot ghost squares
      dX=260;
      dY=750;
      translate(0,0,-map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      factor=4;
      break;
    case 6: //inside 
      dX=width-2;
      dY=hh;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      break;
    case 7: // base
      dX=hw;
      dY=hh;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      break;
    case 8: // outside
      if (dX>width){
        dX = -300;
      }
      dX++;
      dY=hh;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      break;
    case 9: //shot links
      dX=260;
      dY=750;
      translate(0,0,-map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      break;
    case 10: //medium drone dots
      dX=width-2;
      dY=hh;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      break;
    case 11: //medium crosses
      dX=width-2;
      dY=hh;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      factor=2;
      break;
    case 12: //mid-high floating dots perspective
      dX=280;
      dY=300;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      factor=7;
      break;
    case 13: //mid floating mad lines
      if (dX>width){
        dX = -300;
      }
      dX++;
      dY=hh;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      factor=5;
      break;
    case 14: //soft mad rects
      if (dX>width){
        dX = -300;
      }
      dX++;
      dY=hh;
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      factor=6;
      break;
    default:
      translate(0,0,map(dX,0,width, -600, 1200));
      rotateX(map(dY,0,height,PI,-PI));
      break;
  }
  // swipe bands, react
  for (int f=0; f<bands/2; f++){
    for(int i = 0; i < nc; i++){
      // put movement here...
      if (cells[i].b == f){
        cells[i].react(show);
      }
    }
  }
  popMatrix();
  //end
}
 


// *_.-·***--·***·*·-._* ·· *_.-·***--·***·*·-._*  press functions
void mousePressed() {
  for(int i=0;i<nc;i++)
    cells[i].c = 0;
}

void mouseDragged(){
  dY = mouseY;
  dX = mouseX;
  println(dX,"]/t[",dY);
}

void keyPressed(){
  if (key=='b'){
    mode = !mode;
  }
  if (key=='a'){
    show+=1;
    if (show>3) show=0;
  }
  if (key=='d'){
    debug = !debug;
  }
} 



// *_.-·***--·***·*·-._* ·· *_.-·***--·***·*·-._*  funct functions
void updateBuffs(){
  // update spectra
  fft.analyze(spectra);
  // mbuff0
  for (int f=0; f<bands; f++) past_spectra[f] = buff_0[LEN_BUFFER_0-1][f];
  for (int i=LEN_BUFFER_0-1; i>0; i--){
    for (int f=0; f<bands; f++) buff_0[i][f] = buff_0[i-1][f];
  }
  for (int f=0; f<bands; f++) buff_0[0][f] = spectra[f];
  // update mBuff0
  float pow0=0;
  for(int f = 0; f < bands; f++){
    float psum = 0;
    for (int i=0; i<LEN_BUFFER_0-1; i+=1){
      psum = psum + buff_0[i][f];
    }
    float mean = psum/LEN_BUFFER_0;
    float a = map(mean, 0, 0.01, 0,1000);
    mBuff0[f] = a;
    pow0+=mean;
  }
  //mBuff1
  float pow1=0;
  for (int f=0; f<bands; f++) past_spectra[f] = buff_1[LEN_BUFFER_1-1][f];
  for (int i=LEN_BUFFER_1-1; i>0; i--){
    for (int f=0; f<bands; f++) buff_1[i][f] = buff_1[i-1][f];
  }
  for (int f=0; f<bands; f++) buff_1[0][f] = spectra[f];
  // update mBuff1
  for(int f = 0; f < bands; f++){
    float psum = 0;
    for (int i=0; i<LEN_BUFFER_1-1; i+=1){
      psum = psum + buff_1[i][f];
    }
    float mean = psum/LEN_BUFFER_1;
    float a = map(mean, 0, 0.01, 0,1000);
    mBuff1[f] = a;
    pow1+=mean;
  }
  //states machine
  // pow: [0.01 silwnce, .1 med, .2 hard, .5 trash] println(pow0,"···",pow1);
  if(frameCount%66<2){
    if (st==0){
      show = 14;
      if (pow1>0.2){
        st=4;
      }else if (pow1 > 0.15){
        st = 1;
      } else if(frameCount%7200==0){
        st = 3;
      }
    }
    if (st==1){
      show = 13;
      if (pow1 > 0.25){
        st = 2;
      } else if (pow1 > 0.20){
        st = 2;
      } else if (random(1)>0.95){
        st = 3;
      } else if (pow1<0.005)
        st = 0;
    }
    if (st==2){
      show = 9;
      if (pow1<0.18){
        st = 4;
      } else if (pow1 > 0.35){
        st = 1;
      } else if (pow1 > 0.27){
        st = 4;
      }
    }
    if (st==3){
      show = 12;
      if (pow1 > 0.3){
        st = 5;
      } else if(pow1>0.2){
        st = 2;
      } else if (pow1<0.03){
        st = 1;
      }
    }
    if (st==4){
      show = 13;
      if (pow1 < 0.05){
        st = 3;
      } else if (pow1>0.2){
        st = 7;
      }
    }
    if (st==5){
      show = 5;
      if (pow1 < 0.12){
        st = 0;
      } else if(frameCount%7200==0){
        st = 3;
      }
    }
    if (st==6){
      show = 12;
      if (pow1 < 0.6){
        st = 1;
      } else if (pow1>0.2){
        st = 7;
      }
    }
    if (st==7){
      show = 8;
      if (pow1 < 0.07){
        st = 3;
      } else if(pow1>0.35){
        st = 5;      
      } else if(pow1>0.6){
        st = 8;      
      }
    }
    if (st==8){
      show = 9;
      if (pow1 < 0.12){
        st = 7;
      } else if(pow1>0.35){
        st=5;
      } else if((pow1>0.25 )|| (frameCount%7200==0)){
        st = 1;
      }
    }
  }
}


void drawRing(){
  pushMatrix();
  translate(width/2,height/2,-300);
  strokeWeight(5);
  beginShape();
  rotate(-frameCount%720*TWO_PI*2/720);
  for(int f = 0; f < bands; f++){
    stroke(255, 20);
    if (mBuff1[f]>thrsh01*100){ 
      stroke(mBuff1[f], 0,0, 50);
    }
    vertex(150*cos(-PI/2+f*sector),150*sin(-PI/2+f*sector), mBuff1[f]/10);
    rotate(sector);  
  }
  endShape(CLOSE);
  popMatrix();
}


float det(int x1, int y1, int x2, int y2, int x3, int y3) {
  return (float) ((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1));
}


void initGUI(){
}


void initBuffs(){
  for (int b=0; b<LEN_BUFFER_0; b++){
    for (int c=0; c<bands; c++){
      buff_0[b][c] = 0;
    }
  }
  for (int b=0; b<LEN_BUFFER_1; b++){
    for (int c=0; c<bands; c++){
      buff_1[b][c] = 0;
    }
  }
}


void initCells(){
  for(int i = 0; i < nc; i++){
    float a = random(0, TWO_PI);
    float r = map(i, nc, 0, 1, hh);
    int b = int( map(r, hh, 0,0, bands/2) );
    cells[i] = new Cell( hw + r*cos(a), hh + r*sin(a), i, r, a, b );
    // keep on going
  }
  findClosest();
}


void findClosest(){
  for (int i=0; i<nc; i++){
    int cl=0, cl2=0, cl3=0;
    float md = 10000;
    for (int j=0; j<nc; j++){
      float dd = dist(cells[i].pos.x, cells[i].pos.y, cells[j].pos.x, cells[j].pos.y);
      if ( dd < md & dd>.1){
        cl3 = cl2;
        cl2 = cl;
        cl = j;
        md = dd;
      }
    }
    cells[i].setClosest(cl);
    cells[i].setClosest2(cl2);
  }
}
