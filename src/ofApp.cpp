#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  img.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
  bg.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
  sao.setup();
  
  updateImage();
}

//--------------------------------------------------------------
void ofApp::update(){

}

float noise(const glm::vec3& p, int octaves) {
  float res = 0;
  for (int i = 0; i < octaves; i++) {
    res += ofNoise(p * pow(2, i))/pow(2, i);
  }
  return res;
}

void ofApp::updateImage() {
  // Background
  ofColor top, bottom;
  top.setHex(0x9BCDE7);
  bottom.setHex(0xEFE591);
  
  ofMesh bgRect;
  bgRect.addVertex(glm::vec3(0, 0, 0));
  bgRect.addVertex(glm::vec3(ofGetWidth(), 0, 0));
  bgRect.addVertex(glm::vec3(ofGetWidth(), ofGetHeight(), 0));
  bgRect.addVertex(glm::vec3(0, ofGetHeight(), 0));
  bgRect.addColor(top);
  bgRect.addColor(top);
  bgRect.addColor(bottom);
  bgRect.addColor(bottom);
  bgRect.addIndex(0);
  bgRect.addIndex(1);
  bgRect.addIndex(2);
  bgRect.addIndex(0);
  bgRect.addIndex(2);
  bgRect.addIndex(3);
  
  bg.begin();
  bgRect.draw();
  bg.end();
  
  ofColor shadowColor;
  shadowColor.setHex(0x4B1322);
  sao.setShadowColor(shadowColor);
  
  sao.begin();
  sao.setFog(&bg);
  sao.clearDepth();
  
  ofPushMatrix();
  ofTranslate(glm::vec3(ofGetWidth()/2, ofGetHeight()*1.4, -200*5));
  ofRotateXDeg(20);
  
  z = ofRandom(0, 4000);
  
  ofMesh tree = ofMesh::cone(16, 40, 6, 2);
  ofColor treeColor;
  treeColor.setHex(0x3E5C56);
  for (size_t i = 0; i < tree.getNumVertices(); i++) {
    tree.addColor(treeColor);
  }
  
  constexpr int numHills = 5;
  std::array<ofMesh, numHills> hills;
  constexpr size_t w = 200;
  constexpr size_t h = 200;
  float w2 = w/2;
  float h2 = h/2;

  ofColor hillColor;
  hillColor.setHex(0x465652);
//  hillColor.setHex(0x3E5C56);
  
  float xOff = 0;
  for (int i = 0; i < numHills; i++) {
    auto& hill = hills[i];
    
    xOff += ofRandom(-1, 1) * ofGetWidth()*0.7;
    for (size_t x = 0; x < w; x++) {
      for (size_t y = 0; y < h; y++) {
        float mixAmount = float((x-w2)*(x-w2)+(y-h2)*(y-h2))/std::max(float(w2*w2),float(h2*h2));
        float scale = 1 + (mixAmount + 1.0) * (mixAmount - 1.0);
        hill.addVertex(glm::vec3(
          (float(x) - float(w)/2)*15 + xOff,
          (-2 + noise(glm::vec3(float(x)*0.015, float(y)*0.015, z + 1000*i), 5) + scale*10) * 300 - i*200,
          float(y)*5 - i*400));
//        hill.addColor(foreground.getLerped(background, float(i)/float(numHills-1)));
        hill.addColor(hillColor);
      }
    }
    
    for (size_t x = 0; x < w-1; x++) {
      for (size_t y = 0; y < h-1; y++) {
        // Face 1
        hill.addIndex(y * w + x);
        hill.addIndex(y * w + x + 1);
        hill.addIndex((y + 1) * w + x);
        
        // Face 2
        hill.addIndex((y + 1) * w + x);
        hill.addIndex((y + 1) * w + x + 1);
        hill.addIndex(y * w + x + 1);
      }
    }
    
    hill.draw();
    
    constexpr int numTrees = 6400;
    auto faces = hill.getUniqueFaces();
    for (int i = 0; i < numTrees; i++) {
      ofPushMatrix();
      auto* face = &faces[(int)ofRandom(faces.size())];
      while (glm::dot(face->getFaceNormal(), glm::vec3(0, 1, 0)) < 0.1) {
        face = &faces[(int)ofRandom(faces.size())];
      }
      ofTranslate((face->getVertex(0) + face->getVertex(1) + face->getVertex(2)) / 3.0);
      ofTranslate(glm::vec3(0, -20, 0));
      
      auto quat = glm::rotation(glm::vec3(0, 1, 0), face->getFaceNormal());
      auto lessRotated = glm::slerp(quat, glm::quat(), 0.8f);
      ofMultMatrix(glm::toMat4(lessRotated));
      
      ofScale(glm::vec3(1, ofRandom(0.8, 2), 1));
      tree.draw();
      ofPopMatrix();
    }
  }
  
  ofPopMatrix();
  sao.end(&img);
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofClear(0,0,0,1);
  img.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
  updateImage();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
