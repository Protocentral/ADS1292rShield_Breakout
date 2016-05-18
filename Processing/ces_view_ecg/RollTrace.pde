import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.traces.Blank2DTrace;
float time = 0;
float x1,y1;
float x2,y2;

class ScatterTrace extends Blank2DTrace {

  public void TraceDraw(Blank2DTrace.PlotRenderer pr) {
          
       for (int i = 0; i < (pSize-1); i++) {

          pr.canvas.strokeWeight(1.5); 
          
          x1 = pr.valToX(xdata[i]);
          x2 = pr.valToX(xdata[i+1]);
          
          y1 = pr.valToY(ydata[i]);
          y2 = pr.valToY(ydata[i+1]);
     
          pr.canvas.pushStyle();
          
          pr.canvas.stroke(255,0,0);
        
          if(!(x2==0 && y2==0))
          { 
            pr.canvas.line(x1, y1, x2, y2);
          }
          
          pr.canvas.popStyle();                 
      }
     
      for(int j=10;j<pSize-10;j++)
      {
           {
             pr.canvas.strokeWeight(1.5);
             pr.canvas.stroke(0,0,0);
             pr.canvas.line(pr.valToX(time), -250, pr.valToX(time), 250);
           }
      }      
  }
}

class RollTrace
{
  
  public RollTrace()
  {
    
    sTrace1  = new ScatterTrace();

    g.setAxisColour(0, 0, 0);
    g.setFontColour(255, 255, 255);

    g.position.y = 120;
    g.position.x = 70;

    g.setYAxisTickSpacing(10f);
    g.setXAxisTickSpacing(100f);

    gb1 = new  GridBackground(new GWColour(255));
    gb1.setNoGrid();

    g.setBackground(gb1);

    g.setYAxisMin(-100f);
    g.setYAxisMax(100f);
    
    g.setXAxisMin(0);
    g.setXAxisMax(pSize);
    
   g.addTrace(sTrace1);
  }

  
  public void draw() {
      {
        if(start)
        {
          sTrace1.generate(); // regenerate the trace for plotting
        }
        g.draw();
      }
  }
}

