#pragma once

#include "ofMain.h"
#include "util/optional.h"

class SAO {
  ofShader deferred, occlusion, composite;
  ofFbo deferredFbo, occlusionFbo;
  ofFbo* fog = nullptr;
  ofColor shadow;
  
public:
  void setup();
  void begin();
  void clearDepth();
  void setFog(ofFbo* newFog);
  void setShadowColor(ofColor color);
  void end(ofFbo* fbo = nullptr);
};
