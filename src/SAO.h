#pragma once

#include "ofMain.h"

class SAO {
  ofShader deferred, occlusion, composite;
  ofFbo deferredFbo, occlusionFbo;
  
public:
  void setup();
  void begin();
  void end(ofFbo* fbo = nullptr);
};
