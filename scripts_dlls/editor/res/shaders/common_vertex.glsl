#version 130

attribute vec3 gpu_Vertex;
attribute vec2 gpu_TexCoord;
attribute vec4 gpu_Color;
uniform mat4 gpu_ModelViewProjectionMatrix;

varying vec4 color;
varying vec2 texCoord;
varying vec2 pos;

void main(void)
{
	color = gpu_Color;
	texCoord = vec2(gpu_TexCoord);
	gl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex.xy, 0.0, 1.0);
	pos = gl_Position.xy;
}
