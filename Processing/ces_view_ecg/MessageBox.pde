class MessageBox{
  
  public float x, y, w, h;
  boolean colorValue = true;
  
  String BPM = "0.0";
  int padding = 5;

  MessageBox(float _xPos, float _yPos, float _width, float _height) {
    x = _xPos;
    y = _yPos;
    w = _width;
    h = _height;
  }

  public void update() {
  }

  public void draw() {

    pushStyle();
    noStroke();

    // draw background of msgBox
    fill(255,255,255);
    rect(x, y, w, h);

    //draw bg of text fields of msgBox
    strokeWeight(1);
    stroke(color(0, 5, 11));
    fill(color(220,225,242));
    rect(x + padding, y + padding, w - padding*2, h - padding *2);
    textFont(createFont("Arial Bold",15));
    fill(0);
    textAlign(LEFT, TOP);
    text("BPM : "+BPM, x+padding*2, y + padding + 4);
   
    textSize(20);
    popStyle();
  }
   public void msg(double bpm) {  
    BPM = bpm+"";
  }
};

public void msg(double bpm) {
  msgBox.msg(bpm);
}
