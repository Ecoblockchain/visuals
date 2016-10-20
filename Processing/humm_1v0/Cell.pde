
// -------------------       -----                                              ---
//                  ----------    ------                  ------- - -----------
// -                                    ------------------       ---

class Cell{
  PVector pos, pre, pre2;
  float r, a, val, d, val2;
  int n, b, side, cl, cl2;
  float s = 0; // spin velocity
  float c = 0; // current angle
    
  Cell(float x, float y, int n, float r, float a, int b) {
    this.pos = new PVector(x, y);
    this.pre = new PVector(0, 0);
    this.pre2 = new PVector(0, 0);
    this.r = r;
    this.a = a;
    this.b = b;
    this.n=n;  //number
    side = random(2) > 1 ? 1 : -1;
  }
  
  void setClosest(int cl){
    this.cl = cl;
    this.pre = new PVector(cells[cl].pos.x, cells[cl].pos.y);
  }
  
  void setClosest2(int cl2){
    this.cl2 = cl2;
    this.pre2 = new PVector(cells[cl2].pos.x, cells[cl2].pos.y);
  }
  
  void react(int sh){
    //int cb = int( map(a, 0, TWO_PI, 0, bands) );
    this.pos.add(new PVector(random(-1, 1), random(-1,1)));
    this.pre.add(new PVector(random(-1, 1), random(-1,1)));
    val = map(mBuff1[b], 0, 500, 0, 5) + map(mBuff1[bands/2+b], 0, 50, 0, 5);
    val2 = map(mBuff0[b], 0, 500, 0, 5);
    float thrsh02  = thrsh01/(bands-b); 
    if (val>thrsh02){
      //point(pos.x, pos.y, val*30);
      float ra = -PI + random(-TWO_PI, PI);
      s += sp * det(int(pos.x+random(-10,10)), int(pos.y+random(-10,10)), hw, hh, hw+int(r*cos(a-ra)), hh+int(r*cos(a-ra))) / 20000;
      s *= sl;
      c += s;
      d = val*5*b*b/(bands-b) * s + .001;
      if(d>5) d=5;
      val = val*side;
      showMe(sh);
    } else {
    }
  }
  
  void showMe(int sh){
    stroke(255, abs(val*50));
    strokeWeight(2);
    switch(sh){
      case 0: //base lines floating dots
        point( int(pos.x) + d * cos(c/3), int(pos.y) + d * sin(c/3),-250+val*80);
        strokeWeight(1);
        line(int(pos.x), int(pos.y), -250, int(pos.x) + d * cos(TWO_PI - int(c*3)*TWO_PI/factor), int(pos.y) + d * sin(TWO_PI - int(c*3)*TWO_PI/factor),-250);
        break;
      case 1: //base dots floating lines, then squares
        point( int(pos.x) + d * cos(c/4), int(pos.y) + d * sin(c/4), -250);
        strokeWeight(1);
        line(int(pos.x), int(pos.y), -250+val*80, int(pos.x) + d * cos(int(c*3)*TWO_PI/factor), int(pos.y) + d * sin(int(c*3)*TWO_PI/factor),-250+val*80);
        if (abs(val)>3.2){
          stroke(255, (abs(val2)-3) *20 );
          beginShape();
          vertex( int(pos.x), int(pos.y), -250+val*80 );
          vertex( int(pos.x) + d * cos(int(c)*TWO_PI/factor), int(pos.y) + d * sin(int(c)*TWO_PI/factor), -250+val*80 );
          vertex( int(pos.x) + d * cos(int(c)*TWO_PI/factor), int(pos.y) + d * sin(int(c)*TWO_PI/factor), -250+val*75 );
          vertex( int(pos.x), int(pos.y), -250+val*75 );
          endShape(CLOSE);
        }
        break;
      case 2: // base dots floatin lines
        point( int(pos.x) + d * cos(c*4), int(pos.y) + d * sin(c*4),-250);
        if (abs(val)>0.7){
          strokeWeight(1);
          stroke(255, (abs(val2)) *50 );
          line(int(pos.x), int(pos.y), -250+val*80, int(pos.x) + d * cos(TWO_PI-int(c*6)*PI/factor), int(pos.y) + d * sin(TWO_PI-int(c*6)*PI/factor),-250+val*80);
        } else {
          point(int(pos.x) + d * cos(TWO_PI-int(c*2)*TWO_PI/factor), int(pos.y) + d * sin(TWO_PI-int(c*2)*TWO_PI/factor),-250+val*80);
        }
        break;
      case 3: // base lines floating dots again
        strokeWeight(3);
        point( int(pos.x) + d * cos(c/4), int(pos.y) + d * sin(c/4),-250-val*80);
        strokeWeight(1);
        stroke(255, abs(val*50));
        line(int(pos.x), int(pos.y), -250, int(pos.x) + d * cos(int(c/6)*PI/factor), int(pos.y) + d * sin(int(c/6)*PI/factor),-250);
        break;
      case 4: // dots & links
        strokeWeight(3);
        point( int(pos.x), int(pos.y), -250+val*80);
        point( int(pre.x), int(pre.y), -250+val*80);
        point( int(this.pre2.x), int(this.pre2.y), -250+val*80);
        strokeWeight(1);
        if (abs(val)>2.0){
          stroke(255, (abs(val2)-4) *20 );
          beginShape();
          vertex( int(pos.x), int(pos.y), -250+val*80 );
          vertex( int(pre.x), int(pre.y), -250+val*80 );
          vertex( int(this.pre2.x), int(this.pre2.y), -250+val*80 );
          endShape();
          strokeWeight(3);
        } else {
        point( int(pos.x) + d * cos(c*3), int(pos.y) + d * sin(c*3),-250+val*80);
        }
        break;
      case 5:
        showMe(1);
        break;
      case 6:
        showMe(4);
        break;
      case 7:
        showMe(4);
        break;
      case 8:
        showMe(4);
        break;
      case 9:
        showMe(4);
        break;
      case 10:
        showMe(0);
        break;
      case 11:
        showMe(1);
        break;
      case 12:
        showMe(0);
        break;
      case 13:
        showMe(2);
        break;
      case 14:
        showMe(1);
        break;
    }

  }
}
