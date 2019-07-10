#version 330

uniform sampler2DRect depth;
uniform sampler2DRect color;
uniform sampler2DRect occlusion;
uniform sampler2DRect fog;
uniform bool useFog;
uniform vec4 projInfo;

const float Z_NEAR = 1.0;
const float Z_FAR = 1000.0;

const float EDGE_SHARPNESS = 1.0;
const int SCALE = 1;

out vec4 fragColor;

vec3 worldFromScreen(const vec2 screen) {
  float z = Z_NEAR * Z_FAR  / ((Z_NEAR - Z_FAR) * texture(depth, screen).x + Z_FAR);
  return vec3((screen * projInfo.xy + projInfo.zw) * z, z);
}

float blurAO(vec2 screenSpaceOrigin) {
  float sum = texture(occlusion, screenSpaceOrigin).x;
  float originDepth = texture(depth, screenSpaceOrigin).x;
  float totalWeight = 1.0;
  sum *= totalWeight;
  int limit = 4;
  for (int x = -limit; x <= limit; x++) {
    for (int y = -limit; y <= limit; y++) {
      if (x != 0 || y != 0) {
        vec2 samplePosition = screenSpaceOrigin + vec2(float(x * SCALE), float(y * SCALE));
        float ao = texture(occlusion, samplePosition).x;
        float sampleDepth = texture(depth, samplePosition).x;
        // Only use a sample if it isn't out to infinity (depth = 1.0)
        if (sampleDepth < 1.0) {
          // Try to weigh samples closer to the original point more highly
          float weight = 0.3 + 1.0 - (abs(float(x * y)) / float(limit * limit));
          // Also try to weigh samples more highly if they're at similar depths
          weight *= max(0.0, 1.0 - (EDGE_SHARPNESS * 2000.0) * abs(sampleDepth - originDepth)*0.5);
          sum += ao * weight;
          totalWeight += weight;
        }
      }
    }
  }
  const float epsilon = 0.0001;
  return sum / (totalWeight + epsilon);
}

void main() {
  vec2 screenSpaceOrigin = gl_FragCoord.xy;
  vec3 colorAtOrigin = texture(color, screenSpaceOrigin).xyz;
  float occlusionAtOrigin = blurAO(screenSpaceOrigin);
  vec3 rawColor = occlusionAtOrigin * colorAtOrigin;
  
  if (useFog) {
    float dist = max(length(worldFromScreen(gl_FragCoord.xy)) - 20.0, 0.0);
    float FOG_FACTOR = 0.05;
    
    // Beer-Lambert: exponential decay of light going through the medium
    float fogFactor = 1.0 - clamp(exp(-dist * FOG_FACTOR), 0.0, 1.0);
    fragColor = vec4(texture(fog, gl_FragCoord.xy).xyz * fogFactor + rawColor * (1.0 - fogFactor), 1.0);
  } else {
    fragColor = vec4(rawColor, 1.0);
  }
}
