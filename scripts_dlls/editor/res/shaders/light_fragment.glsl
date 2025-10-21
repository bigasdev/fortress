#version 130

in vec4 color;
in mat4 viewprojectionmatrix;
in vec2 texCoord;
in vec2 pos;

out vec4 fragColor;

uniform sampler2D tex;
uniform float pos_x = 0.0;
uniform float pos_y = 0.0;
uniform float window_x = 400.0;
uniform float window_y = 200.0;
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0); // Color of the light
uniform float lightRadius = 1.95; // Radius of the light effect
uniform vec3 ambientColor = vec3(0.65, 0.65, 0.65); // Ambient light color

void main(void)
{
    vec4 texColor = texture2D(tex, texCoord) * color;

    // Transform the light position from world space to screen space
    vec4 lightPosScreen = viewProjectionMatrix * vec4(vec2(pos_x, pos_y), 0.0, 1.0)*5;
    vec2 lightPosScreen2D = lightPosScreen.xy / lightPosScreen.w;

    // Calculate the distance from the light source
    vec2 fragCoord2D = gl_FragCoord.xy / vec2(window_x, window_y); // Assuming a fixed resolution
    float distance = distance(fragCoord2D, lightPosScreen2D);
    float value = 1.0 - smoothstep(-0.2, lightRadius, distance);

    // Calculate the light intensity based on distance
    float intensity = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

    // Combine the light color with the texture color
    //vec3 lighting = ambientColor * lightColor * clamp(value, 0.0, 1.0);
    vec3 lighting = ambientColor;

    // Output the final color
    fragColor = vec4(texColor.rgb * lighting, texColor.a);

    // we can use this to single color shaders
    // if(texColor.a > 0.5)
       // fragColor = vec4(1.0,0.0,0.0,1.0);
}
