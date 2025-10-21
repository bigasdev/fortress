#version 130

in vec4 color;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform vec4 outlineColor = vec4(1.0, 1.0, 0.0, 1.0); // Default white color
uniform float outlineThickness = 0.00295; // Default thickness of 1

void main(void)
{
  vec4 color_frag = texture2D(tex, texCoord);
  float alpha = color_frag.a;

  alpha = max(alpha, texture2D(tex, texCoord + vec2(outlineThickness, 0)).a);
  alpha = max(alpha, texture2D(tex, texCoord + vec2(-outlineThickness, 0)).a);
  alpha = max(alpha, texture2D(tex, texCoord + vec2(0, outlineThickness)).a);
  alpha = max(alpha, texture2D(tex, texCoord + vec2(0, -outlineThickness)).a);

  if(color_frag.a == 0.0 && alpha >= 1.0){
      fragColor.rgb = outlineColor.rgb;
      fragColor.a = alpha;
    }else{
      fragColor = color_frag;
      }
}
