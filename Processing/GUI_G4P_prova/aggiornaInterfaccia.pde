void aggiornaInterfaccia()
{
  //Creo il grafico:
  int g_w = 700;
  int g_h = 500;
  PGraphics graficoProfiloSfondo = createGraphics(g_w, g_h);
  graficoProfiloSfondo.beginDraw();
    // AXIS:
    graficoProfiloSfondo.stroke(0);
    graficoProfiloSfondo.strokeWeight(3);  // Thicker
    graficoProfiloSfondo.line(startX,startY, stopX, startY); // X AXIS
    graficoProfiloSfondo.line(startX,startY, startX, stopY); // Y AXIS
    //lineette
    graficoProfiloSfondo.strokeWeight(1);
    for(int i=stepX; i<stopX-startX-10; i=i+stepX)
      graficoProfiloSfondo.line(startX+i,startY-5,startX+i,startY+5);
    for(int i=stepY; i<startY-stopY-10; i=i+stepY)
      graficoProfiloSfondo.line(startX-5,startY-i,startX+5,startY-i);
  
    graficoProfiloSfondo.stroke(150);
    graficoProfiloSfondo.line(startX,startY,stepX*2.5+startX, startY-stepY*3);
    graficoProfiloSfondo.line(stepX*2.5+startX, startY-stepY*3, stepX*6+startX, startY-stepY*4);
    graficoProfiloSfondo.line(stepX*6+startX, startY-stepY*4, stepX*8+startX, startY-stepY*5);
    graficoProfiloSfondo.line(stepX*8+startX, startY-stepY*5, stepX*10+startX, startY-stepY*4);
    graficoProfiloSfondo.line(stepX*10+startX, startY-stepY*4, stepX*12+startX, startY-stepY*2);
  
    graficoProfiloSfondo.stroke(180);
    graficoProfiloSfondo.line(startX+stepX*2.5,stopY+30,startX+stepX*2.5,startY);
    graficoProfiloSfondo.line(startX+stepX*6,stopY+30,startX+stepX*6,startY);
    graficoProfiloSfondo.line(startX+stepX*10,stopY+30,startX+stepX*10,startY);
    
    //creo l'insieme di punti che mi dà il grafico
    graficoProfiloSfondo.stroke(0);
    graficoProfiloSfondo.strokeWeight(2);
    for(int i=0; i<table.getRowCount(); i++)
    {
      TableRow row = table.getRow(i);
      X = row.getInt("time");
      Y = (int)row.getFloat("temperature");
      
      line(oldX, oldY, X, Y);
      oldX = X;
      oldY = Y;
    }
    if(table.getRowCount() > 0)
    {
      TableRow first_row = table.getRow(0);
      oldX = first_row.getInt("time");
      oldY = (int)first_row.getFloat("temperature");
    }
    
  graficoProfiloSfondo.endDraw();
  image(graficoProfiloSfondo, 10, 0);
}