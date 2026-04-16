#version 460 core
in vec2 TexCoord;
flat in int Visible;
out vec4 FragColor;
uniform sampler2D aTexture;
void main()
{ 


  if(Visible == 1){
    FragColor = texture(aTexture, TexCoord);
  }

}
