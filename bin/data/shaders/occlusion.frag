#version 330

uniform sampler2DRect depth;
uniform vec4 projInfo;
uniform ivec2 screenSize;
uniform float scale;

out vec4 fragColor;

const int NUM_SAMPLES = 37;
const int NUM_SPIRAL_TURNS = 7;
const float EPSILON = 0.1;
const float BIAS = 0.7;
const float WORLD_SPACE_RADIUS = 60.0; // radius of influence in world space
const float INTENSITY = 30.0;
const float M_PI = 3.1415926535897932384626433832795;

const float Z_NEAR = 1.0;
const float Z_FAR = 1000.0;

float random(vec3 scale, float seed) {
  return fract(sin(dot(gl_FragCoord.xyz + seed, scale)) * 43758.5453 + seed);
}

vec3 worldFromScreen(const vec2 screen) {
  float z = Z_NEAR * Z_FAR  / ((Z_NEAR - Z_FAR) * texture(depth, screen).x + Z_FAR);
  return vec3((screen * projInfo.xy + projInfo.zw) * z, z);
}

vec3 normalFromWorld(const vec3 position) {
  return normalize(cross(dFdy(position), dFdx(position)));
}

vec3 getOffsetPositionVS(vec2 screenOrigin, vec2 unitOffset, float screenSpaceRadius) {
  // Offset by screenSpaceRadius pixels in the direction of unitOffset
  vec2 screenOffset = screenOrigin + screenSpaceRadius * unitOffset;
  
  // Get the world coordinate from the offset screen space coordinate
  return worldFromScreen(screenOffset);
}

void main() {
  ivec2 pixel = ivec2(gl_FragCoord.xy);
  vec2 uv = gl_FragCoord.xy;
  
  vec3 worldSpaceOrigin = worldFromScreen(uv);
  vec3 normalAtOrigin = normalFromWorld(worldSpaceOrigin);
  
  vec3 randomScale = vec3(12.9898, 78.233, 151.7182);
  vec3 sampleNoise = vec3(
                          random(randomScale, 0.0),
                          random(randomScale, 1.0),
                          random(randomScale, 2.0));
  
  float initialAngle = 2.0 * M_PI * sampleNoise.x;
  
  // radius of influence in screen space
  float screenSpaceSampleRadius  = 100.0 * WORLD_SPACE_RADIUS / worldSpaceOrigin.z * scale;
//  float screenSpaceSampleRadius = WORLD_SPACE_RADIUS;
  
  float occlusion = 0.0;
  for (int sampleNumber = 0; sampleNumber < NUM_SAMPLES; sampleNumber++) {
    // Step 1:
    // Looking at the 2D image of the scene, sample the points surrounding the current one
    // in a spiral pattern
    
    float sampleProgress = (float(sampleNumber) + 0.5) * (1.0 / float(NUM_SAMPLES));
    float angle = sampleProgress * (float(NUM_SPIRAL_TURNS) * 2.0 * M_PI) + initialAngle;
    
    float sampleDistance = sampleProgress * screenSpaceSampleRadius;
    vec2 angleUnitVector = vec2(cos(angle), sin(angle));
    
    // Step 2:
    // Get the 3d coordinate corresponding to the sample on the spiral
    vec3 worldSpaceSample = getOffsetPositionVS(uv, angleUnitVector, sampleDistance);
    
    // Step 3:
    // Approximate occlusion from this sample
    vec3 originToSample = worldSpaceSample - worldSpaceOrigin;
    float squaredDistanceToSample = dot(originToSample, originToSample);
    
    // vn is proportional to how close the sample point is to the origin point along
    // the normal at the origin
    float vn = dot(normalize(originToSample), normalAtOrigin) - BIAS;
    
    // f is proportional to how close the sample point is to the origin point in the
    // sphere of influence in world space
    float radiusSquared = screenSpaceSampleRadius * screenSpaceSampleRadius;
    float f = max(radiusSquared - squaredDistanceToSample, 0.0) / radiusSquared;
    float sampleOcclusion =  f * f * f * max(vn / (EPSILON + squaredDistanceToSample), 0.0);
    
    // Accumulate occlusion
    occlusion += sampleOcclusion;
  }
  
  occlusion = 1.0 - occlusion / (4.0 * float(NUM_SAMPLES));
  
  occlusion = clamp(pow(occlusion, 1.0 + INTENSITY), 0.0, 1.0);
  float rawZ = texture(depth, uv).x;
  if (abs(dFdx(rawZ)) < 0.5) {
    occlusion -= dFdx(occlusion) * (float(pixel.x & 1) - 0.5);
  }
  if (abs(dFdy(rawZ)) < 0.5) {
    occlusion -= dFdy(occlusion) * (float(pixel.y & 1) - 0.5);
  }
  
  fragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
