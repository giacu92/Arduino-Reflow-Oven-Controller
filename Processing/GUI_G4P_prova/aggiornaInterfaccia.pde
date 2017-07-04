void aggiornaInterfaccia()
{
  // AXIS:
  stroke(0);
  strokeWeight(3);  // Thicker
  line(startX,startY, stopX, startY); // X AXIS
  line(startX,startY, startX, stopY); // Y AXIS
  //lineette
  strokeWeight(1);
  for(int i=stepX; i<stopX-startX-10; i=i+stepX)
    line(startX+i,startY-5,startX+i,startY+5);
  for(int i=stepY; i<startY-stopY-10; i=i+stepY)
    line(startX-5,startY-i,startX+5,startY-i);
  
  stroke(150);
  line(startX,startY,stepX*2.5+startX, startY-stepY*3);
  line(stepX*2.5+startX, startY-stepY*3, stepX*6+startX, startY-stepY*4);
  line(stepX*6+startX, startY-stepY*4, stepX*8+startX, startY-stepY*5);
  line(stepX*8+startX, startY-stepY*5, stepX*10+startX, startY-stepY*4);
  line(stepX*10+startX, startY-stepY*4, stepX*12+startX, startY-stepY*2);
  
  stroke(180);
  line(startX+stepX*2.5,stopY+30,startX+stepX*2.5,startY);
  line(startX+stepX*6,stopY+30,startX+stepX*6,startY);
  line(startX+stepX*10,stopY+30,startX+stepX*10,startY);
}