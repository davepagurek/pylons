#include "SAO.h"

void SAO::setup() {
  {
    ofFbo::Settings settings;
    settings.width = ofGetWidth();
    settings.height = ofGetHeight();
    settings.useDepth = true;
    settings.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    settings.internalformat = GL_RGBA32F;
    settings.depthStencilAsTexture = true;
    deferredFbo.allocate(settings);
    deferredFbo.setUseTexture(true);
  }
  
  {
    ofFbo::Settings settings;
    settings.width = ofGetWidth();
    settings.height = ofGetHeight();
    settings.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    settings.internalformat = GL_RGBA32F;
    occlusionFbo.allocate(settings);
    occlusionFbo.setUseTexture(true);
  }
  
  deferred.load("shaders/deferred");
  occlusion.load("shaders/occlusion");
  composite.load("shaders/composite");
}

void SAO::begin() {
  deferredFbo.begin();
  deferredFbo.activateAllDrawBuffers();
  ofClear(0);
  //  deferredFbo.clearDepthBuffer(1);
  ofEnableDepthTest();
  deferred.begin();
}

void SAO::end(ofFbo* fbo) {
  // End deferred render
  deferred.end();
  deferredFbo.end();
  
  // AO render
  ofDisableDepthTest();
  occlusionFbo.begin();
  occlusion.begin();
  occlusion.setUniformTexture("depth", deferredFbo.getDepthTexture(), 1);
  auto proj = ofGetCurrentMatrix(OF_MATRIX_PROJECTION);
  occlusion.setUniform4f("projInfo",
                         -2.0f / (ofGetWidth() * proj[0][0]),
                         -2.0f / (ofGetHeight() * proj[1][1]),
                         (1.0f - proj[0][2]) / proj[0][0],
                         (1.0f + proj[1][2]) / proj[1][1]);
  occlusion.setUniform2i("screenSize", ofGetWidth(), ofGetHeight());
  deferredFbo.draw(0, 0);
  occlusion.end();
  occlusionFbo.end();
  
  // Final render
  if (fbo) fbo->begin();
  composite.begin();
  composite.setUniformTexture("depth", deferredFbo.getDepthTexture(), 1);
  composite.setUniformTexture("color", deferredFbo.getTexture(), 2);
  composite.setUniformTexture("occlusion", occlusionFbo.getTexture(), 3);
  deferredFbo.draw(0, 0);
  composite.end();
  if (fbo) fbo->end();
  
  ofEnableDepthTest();
}
