#version 460 core 
layout(location = 0) in vec2 aPos;         // Vertex Position
layout(location = 1) in vec2 aTexCoord;    // Texture Coordinates
layout(location = 2) in int aVisible; //Visibility flag
out vec2 TexCoord;       // Texture coords 
flat out int Visible;	// Visibility coords

uniform mat3 transform; 

void main() 
{
	vec2 Pos = vec2(transform * vec3(aPos, 1.0));
	gl_Position = vec4(Pos, 0.0, 1.0); 
	TexCoord = aTexCoord; 
	Visible = aVisible;
}
