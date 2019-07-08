#pragma once

#include "ofMain.h"
#include "SAO.h"

float noise(const glm::vec3& p, int octaves);

class ofApp : public ofBaseApp{
private:
  ofFbo img;
  SAO sao;
  float z;

public:
  void setup();
  void update();
  void draw();
  
  void updateImage();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseEntered(int x, int y);
  void mouseExited(int x, int y);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
};
