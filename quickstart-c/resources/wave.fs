#version 330

// Input uniform values
uniform float time;
uniform vec2 resolution;
uniform vec2 pointer;

// Output fragment color
out vec4 finalColor;

// Palette function from original shader
vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);
    return a + b * cos(1.28318 * (c * t + d));
}

void main() {
    // gl_FragCoord contains window-relative coordinates
    // We normalize to center (0,0) and adjust for aspect ratio
    vec2 uv = (gl_FragCoord.xy * 2.0 - resolution.xy) / resolution.y;

    // --- Cursor Rotation Logic ---
    float cursorAngle = 0.0;
    
    // Avoid division by zero/glitches at exact center
    if (length(pointer) > 0.001) {
        cursorAngle = atan(pointer.y, pointer.x);
    }
    
    float cosAngle = cos(cursorAngle);
    float sinAngle = sin(cursorAngle);
    vec2 rotatedUV;
    
    // Apply clockwise rotation matrix
    rotatedUV.x = uv.x * cosAngle * 0.1 + uv.y * sinAngle * 0.1;
    rotatedUV.y = -uv.x * sinAngle * 0.1 + uv.y * cosAngle * 0.1;
    
    uv = rotatedUV;
    vec2 uv0 = uv;
    vec3 color = vec3(0.0);
    
    // --- Fractal Pattern Loop ---
    for(float i = 0.0; i < 4.0; i++) {
        uv = fract(uv * 1.5) - 0.5;
        
        float d = length(uv) * exp(-length(uv0));
        
        vec3 col = palette(length(uv0) + i * 0.4 + time * 0.4 + pointer.y);
        
        d = sin(d * (8.0 + pointer.x * 2.0) + time) / 8.0;
        d = abs(d);
        d = pow(0.01 / d, 1.2);
        
        color += col * d;
    }
    
    finalColor = vec4(color, 1.0);
}
