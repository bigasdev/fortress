#version 130

in vec4 color;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform float r = 1.0;
uniform float g = 1.0;
uniform float b = 0.0;
uniform float width = 1.0f;
uniform float height = 1.0f;

const float SMALL_NUMBER = 0.0001f;

void main(void)
{
  vec4 pixel = texture2D(tex, texCoord);

  if(pixel.a < SMALL_NUMBER){
    vec2 offset = vec2(1.0/width, 0.0);
    float left = texture2D(tex, texCoord - offset).a;
    float right = texture2D(tex, texCoord + offset).a;

    offset.x = 0.0;
    offset.y = 1.0/height;
    float top = texture2D(tex, texCoord - offset).a;
    float bottom = texture2D(tex, texCoord + offset).a;

    float a = step(SMALL_NUMBER, left+right+top+bottom);
    pixel = mix(pixel, vec4(vec3(r,g,b), 1.0), a);
  }

  fragColor = pixel;
}
