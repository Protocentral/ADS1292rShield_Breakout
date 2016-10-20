//////////////////////////////////////////////////////////////////////////////////////////
//
//   Graph Class
//      - Draw Axes Lines
//      - Set the Limits for each axes
//      - Function to plot the current pressure value
//
//   Created : Balasundari, May 2016
//
//   
/////////////////////////////////////////////////////////////////////////////////////////

class Graph
{
  int xDiv=5, yDiv=5;                                        // Number of sub divisions
  int xPos, yPos;                                            // location of the top left corner of the graph  
  int Width, Height;                                         // Width and height of the graph
  color GraphColor;                                        // Color for the trace
  float yMax=1024, yMin=0;                                 // Default axis dimensions
  float xMax=10, xMin=0;
  PFont Font;                                              // Selected font used for text 

  /******************** The main declaration function ************************************/

  Graph(int x, int y, int w, int h) {               
    xPos = x;
    yPos = y;
    Width = w;
    Height = h;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  The LineGraph() is called repeatedly by the main draw function
  //  This module trace the graph with the buffer values passed from the main function
  //  Functions:
  //    - stroke(int color)        :  Sets the color used to draw lines and borders around shapes.
  //    - strokeWeight(int value)  :  Sets the width of the stroke used for lines, points, and the border around shapes.
  //    - smooth()                 :  Draws all geometry with smooth (anti-aliased) edges.
  //    - line(x1,y1,x2,y2)        :  Draws a line (a direct path between two points) to the screen. The two points are taken from the parameter.
  //    - rect(x,y,w,h)            :  Draws a rectangle to the screen in the position specified in the parameters.
  //    - fill(int color)          :  Sets the color used to fill shapes
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void LineGraph(float[] x, float[] y) {

    for (int i=0; i<(x.length-1); i++)
    {
      smooth();
      strokeWeight(3);
      stroke(GraphColor);
      // x & y points to draw line  
      line(
        xPos+(x[i]-x[0])/(x[x.length-1]-x[0])*Width, 
        yPos+(Height)-(y[i]/(yMax-yMin)*Height)+(yMin)/(yMax-yMin)*Height, 
        xPos+(x[i+1]-x[0])/(x[x.length-1]-x[0])*Width, 
        yPos+(Height)-(y[i+1]/(yMax-yMin)*Height)+(yMin)/(yMax-yMin)*Height
      );
    }

    stroke(0);
    fill(0);
    // Rectangle is drawn to differentiate previous line and current line
    rect(xPos-22+((time-2)-x[0])/(x[x.length-1]-x[0])*Width, 0, 50, height);    
  }
}