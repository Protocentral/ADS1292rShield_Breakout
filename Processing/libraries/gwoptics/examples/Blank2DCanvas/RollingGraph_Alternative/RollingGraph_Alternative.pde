 import org.gwoptics.graphics.graph2D.Graph2D;
  import org.gwoptics.graphics.graph2D.traces.Blank2DTrace;

  float t = 0f;
  float xmin = -5f;
  float xmax = 1f;
  float xinc = 1;
  float preX = 0;
  float preY = 0;
  float x1=0 ,y1=0;
  ScatterTrace sTrace;
  Graph2D g;
  
  class Point2D{
    public float X,Y;
    public Point2D(float x, float y){
    X=x; Y=y;
     println("X = "+X+" Y = "+Y);
    }
  }

  class ScatterTrace extends Blank2DTrace{
    private ArrayList _data;
    private float pSize = 5f;
    
    public ScatterTrace(){
      _data = new ArrayList();
      
    }
    
    public void addPoint(float x, float y){ _data.add(new Point2D(x,y)); } 
  
    private void drawPoint(Point2D p, Blank2DTrace.PlotRenderer pr){
      
      preX = x1;
      preY = y1;
      
      x1 = pr.valToX(p.X);
      y1 = pr.valToY(p.Y);
      
      pr.canvas.strokeWeight(1f);
      pr.canvas.pushStyle();
      pr.canvas.stroke(255,0,0);
      pr.canvas.line(preX, preY, x1 , y1);
     // pr.canvas.line(preX, preY, x1 , y1);   
      pr.canvas.popStyle();
      
    }
    
    public void TraceDraw(Blank2DTrace.PlotRenderer pr) {
      if(_data != null){   
        
        x1 = pr.valToX(((Point2D)_data.get(0)).X);
        y1 = pr.valToY(((Point2D)_data.get(0)).Y);
        for (int i = 1; i < _data.size(); i++) {
        Point2D  pt = (Point2D)_data.get(i);
          // don't draw if not visible on the axis
          if(pt.X > g.getXAxis().getMinValue()){
            drawPoint(pt, pr);
          }          
        }
      }
    }
  }
  
    
  void setup(){
    size(600,500, OPENGL);
      //frameRate(100);  
    sTrace  = new ScatterTrace();
    
    g = new Graph2D(this, 400,400, false);
    g.setAxisColour(220, 220, 220);
    g.setFontColour(255, 255, 255);
        
    g.position.y = 50;
    g.position.x = 100;
        
    g.setYAxisTickSpacing(1f);
    g.setXAxisTickSpacing(1f);
    
    g.setXAxisMinorTicks(1);
    g.setYAxisMinorTicks(1);
    
    g.setYAxisMin(-1f);
    g.setYAxisMax(1f);
        
    g.setXAxisLabelAccuracy(0);
    
    g.addTrace(sTrace);
    
  }
  
  void draw(){
    background(0);
    
    t += 1.0f/(float)frameRate;
    println(t);
    g.setXAxisMin(xmin + t*xinc);
    g.setXAxisMax(xmax + t*xinc);
    
    sTrace.addPoint(t,sin(t));
    sTrace.generate(); // regenerate the trace for plotting
    
    g.draw();
  }