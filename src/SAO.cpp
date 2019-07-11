#include "SAO.h"
#include "constants.h"

void SAO::setup() {
  {
    ofFbo::Settings settings;
    settings.width = ofGetWidth() * SCALE;
    settings.height = ofGetHeight() * SCALE;
    settings.useDepth = true;
    settings.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    settings.internalformat = GL_RGBA32F;
    settings.depthStencilAsTexture = true;
    deferredFbo.allocate(settings);
    deferredFbo.setUseTexture(true);
  }
  
  {
    ofFbo::Settings settings;
    settings.width = ofGetWidth() * SCALE;
    settings.height = ofGetHeight() * SCALE;
    settings.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    settings.internalformat = GL_RGBA32F;
    occlusionFbo.allocate(settings);
    occlusionFbo.setUseTexture(true);
  }
  
  deferred.load("shaders/deferred");
  occlusion.load("shaders/occlusion");
  composite.load("shaders/composite");
  
  shadow.setHex(0x000000);
}

void SAO::begin() {
  deferredFbo.begin();
  deferredFbo.activateAllDrawBuffers();
  ofClear(0);
  //  deferredFbo.clearDepthBuffer(1);
  ofEnableDepthTest();
  deferred.begin();
  ofPushMatrix();
  ofScale(glm::vec3(SCALE, SCALE, SCALE));
}

void SAO::clearDepth() {
  deferredFbo.clearDepthBuffer(1);
}

void SAO::setFog(ofFbo* newFog) {
  fog = newFog;
}

void SAO::setShadowColor(ofColor color) {
  shadow = color;
}

void SAO::end(ofFbo* fbo) {
  // End deferred render
  deferred.end();
  deferredFbo.end();
  ofPopMatrix();
  
  // AO render
  ofDisableDepthTest();
  occlusionFbo.begin();
  occlusion.begin();
  occlusion.setUniformTexture("depth", deferredFbo.getDepthTexture(), 1);
  auto proj = ofGetCurrentMatrix(OF_MATRIX_PROJECTION);
  occlusion.setUniform4f("projInfo",
                         -2.0f / (ofGetWidth()*SCALE * proj[0][0]),
                         -2.0f / (ofGetHeight()*SCALE * proj[1][1]),
                         (1.0f - proj[0][2]) / proj[0][0],
                         (1.0f + proj[1][2]) / proj[1][1]);
  occlusion.setUniform2i("screenSize", ofGetWidth(), ofGetHeight());
  occlusion.setUniform1f("scale", SCALE);
  deferredFbo.draw(0, 0);
  occlusion.end();
  occlusionFbo.end();
  
  // Final render
  if (fbo) fbo->begin();
  composite.begin();
  composite.setUniformTexture("depth", deferredFbo.getDepthTexture(), 1);
  composite.setUniformTexture("color", deferredFbo.getTexture(), 2);
  composite.setUniformTexture("occlusion", occlusionFbo.getTexture(), 3);
  composite.setUniform1i("useFog", static_cast<bool>(fog));
  if (fog) {
    composite.setUniformTexture("fog", fog->getTexture(), 4);
  }
  composite.setUniform4f("projInfo",
                         -2.0f / (ofGetWidth()*SCALE * proj[0][0]),
                         -2.0f / (ofGetHeight()*SCALE * proj[1][1]),
                         (1.0f - proj[0][2]) / proj[0][0],
                         (1.0f + proj[1][2]) / proj[1][1]);
  composite.setUniform3f("shadowColor", glm::vec3(shadow.r, shadow.g, shadow.b) / shadow.limit());
  deferredFbo.draw(0, 0);
  composite.end();
  if (fbo) fbo->end();
  
  ofEnableDepthTest();
}
