
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include "stb_image.h" 
#include "./Graphics_Libraries/glad/include/glad/glad.h" 
#include "./Graphics_Libraries/glfw-3.4/include/GLFW/glfw3.h"




void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/*Callback functions*/
/*
 * framebuffer_size_callback: Callback function to adjust the graphics upon window resizing
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
  if(width > height){
    glViewport(0, 0, width, width);
  }
  else{
    glViewport(0, 0, height, height);

  }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  keys_t* keys = (keys_t*)glfwGetWindowUserPointer(window);
  if (keys == NULL){return;}
  
  char* string = keys->string;
  char* truth = keys->truth;


  switch(key){
  case GLFW_KEY_W:
      process_keys('w', action, string, truth);
      break;
  case GLFW_KEY_A:
      process_keys('a', action, string, truth);
      break;
  case GLFW_KEY_S:
      process_keys('s', action, string, truth);
      break;
  case GLFW_KEY_D:
      process_keys('d', action, string, truth);
      break;
  case GLFW_KEY_Q:
      process_keys('q', action, string, truth);
      break;
  case GLFW_KEY_E:
      process_keys('e', action, string, truth);
      break;
  case GLFW_KEY_Z:
      process_keys('z', action, string, truth);
      break;
  case GLFW_KEY_C:
      process_keys('c', action, string, truth);
      break;
  case GLFW_KEY_R:
      process_keys('r', action, string, truth);
      break;
    case GLFW_KEY_SPACE:
      process_keys('j', action, string, truth);
      break;
  }
}


/*****update_map helper functions*********/

/*
 *  Returns vertices (assumed square) based on texture/location data 
 */
void square_vertices_from_info(int numcols, int numrows, int numtext_perside, int texture_xindex, int texture_yindex, int xindex, int yindex, float* vertices_out){
  float square_sidelength;
  float x_offset, y_offset;
  if (numcols < numrows){
    square_sidelength =  2.0f / (float)numrows;

    x_offset = -1.0f;
    y_offset = -1.0f;
  } else {
    square_sidelength =  2.0f / (float)numcols;

    y_offset = -1.0f;
    x_offset = -1.0f;
  }

  float xincrement = (float) xindex * square_sidelength;
  float yincrement = (float) yindex * square_sidelength;
  float texturewidth_offset = 1.0f/(float)numtext_perside;
  vertices_out[0] = x_offset + xincrement;
  vertices_out[1] = y_offset + yincrement;
  vertices_out[2] = (float)texture_xindex/(float)numtext_perside;
  vertices_out[3] = (float)texture_yindex/(float)numtext_perside;
  vertices_out[4] = x_offset + xincrement + square_sidelength;
  vertices_out[5] = y_offset + yincrement;
  vertices_out[6] = (float)texturewidth_offset + (float)texture_xindex/(float)numtext_perside;
  vertices_out[7] = (float)texture_yindex/(float)numtext_perside;
  vertices_out[8] = x_offset + xincrement;
  vertices_out[9] = y_offset + yincrement + square_sidelength;
  vertices_out[10] = (float)texture_xindex/(float)numtext_perside;
  vertices_out[11] = (float)texturewidth_offset + (float)texture_yindex/(float)numtext_perside;
  vertices_out[12] = x_offset + xincrement + square_sidelength;
  vertices_out[13] = y_offset + yincrement + square_sidelength;
  vertices_out[14] = (float)texturewidth_offset + (float)texture_xindex/(float)numtext_perside;
  vertices_out[15] = (float)texturewidth_offset + (float)texture_yindex/(float)numtext_perside;
}
/*
  * Get Texture: takes char and returns texture indices
  */
void get_textureIdx(char ch, int (*index)[2]){
  switch (ch){
    case ' ':
      (*index)[0] = 0;
      (*index)[1] = 0;
      break;
    case '#':
      (*index)[0] = 1;
      (*index)[1] = 0;
      break;
    case '*':
      (*index)[0] = 2;
      (*index)[1] = 0;
      break;
    case '~':
      (*index)[0] = 3;
      (*index)[1] = 0;
      break;
    case '?':
      (*index)[0] = 4;
      (*index)[1] = 0;
      break;
    case '-':
      (*index)[0] = 5;
      (*index)[1] = 0;
      break;
    case '|':
      (*index)[0] = 0;
      (*index)[1] = 1;
      break;
    case '+':
      (*index)[0] = 1;
      (*index)[1] = 1;
      break;
    case '.':
      (*index)[0] = 2;
      (*index)[1] = 1;
      break;
    case 'A':
      (*index)[0] = 3;
      (*index)[1] = 1;
      break;
    case 'B':
      (*index)[0] = 4;
      (*index)[1] = 1;
      break;
    case 'C':
      (*index)[0] = 5;
      (*index)[1] = 1;
      break;
    case 'D':
      (*index)[0] = 0;
      (*index)[1] = 2;
      break;
    case 'E':
      (*index)[0] = 1;
      (*index)[1] = 2;
      break;
    case 'F':
      (*index)[0] = 2;
      (*index)[1] = 2;
      break;
    case 'G':
      (*index)[0] = 3;
      (*index)[1] = 2;
      break;
    case 'H':
      (*index)[0] = 4;
      (*index)[1] = 2;
      break;
    case 'I':
      (*index)[0] = 5;
      (*index)[1] = 2;
      break;
    case 'J':
      (*index)[0] = 0;
      (*index)[1] = 3;
      break;
    case 'K':
      (*index)[0] = 1;
      (*index)[1] = 3;
      break;
    case 'L':
      (*index)[0] = 2;
      (*index)[1] = 3;
      break;
    case 'M':
      (*index)[0] = 3;
      (*index)[1] = 3;
      break;
    case 'N':
      (*index)[0] = 4;
      (*index)[1] = 3;
      break;
    case 'O':
      (*index)[0] = 5;
      (*index)[1] = 3;
      break;
    case 'P':
      (*index)[0] = 0;
      (*index)[1] = 4;
      break;
    case 'Q':
      (*index)[0] = 1;
      (*index)[1] = 4;
      break;
    case 'R':
      (*index)[0] = 2;
      (*index)[1] = 4;
      break;
    case 'S':
      (*index)[0] = 3;
      (*index)[1] = 4;
      break;
    case 'T':
      (*index)[0] = 4;
      (*index)[1] = 4;
      break;
    case 'U':
      (*index)[0] = 5;
      (*index)[1] = 4;
      break;
    case 'V':
      (*index)[0] = 0;
      (*index)[1] = 5;
      break;
    case 'W':
      (*index)[0] = 1;
      (*index)[1] = 5;
      break;
    case 'X':
      (*index)[0] = 2;
      (*index)[1] = 5;
      break;
    case 'Y':
      (*index)[0] = 3;
      (*index)[1] = 5;
      break;
    case 'Z':
      (*index)[0] = 4;
      (*index)[1] = 5;
      break;
    default:
      (*index)[0] = 9;
      (*index)[1] = 9;
      break;
  }

}


/*
  * Frees allocated memory
  */
void free_map_buffers(graphicEnv* context) {
  glDeleteVertexArrays(1, &context->VAO);
  glDeleteBuffers(1, &context->VBO);
  glDeleteBuffers(1, &context->EBO);
  glDeleteBuffers(1, &context->VBO_visibility);
  free(context->vertices);
  free(context->indices);
  free(context->visibility_data);
  free(context->visibleMapPrev);
}


/*
  * Initializes buffers used in rendering VAO VBO & EBO
  */
int init_map_buffers(graphicEnv* context){

  //Creating a vertex array and binding the buffer to it
  //Loading the buffer with the vertices we specified
  //Specifying the attributes of the vertices contained in the buffer
  //Enabling the vertex attribute array
  //Unbinding array and buffer


  glGenVertexArrays(1, &context->VAO);
  glGenBuffers(1, &context->VBO);
  glGenBuffers(1, &context->VBO_visibility);

  glGenBuffers(1, &context->EBO);
  // Confirm VAO was set
  if(context->VAO == 0 || context->VBO == 0 || context->EBO == 0){
    fprintf(stderr, "Error setting VAO/VBO/EBO: VAO: %d, VBO: %d, EBO: %d", context->VAO, context->VBO, context->EBO);
    exit(1);
  } 

  // Check that glBindVertexArray is not NULL
  if (!glBindVertexArray) {
    fprintf(stderr, "ERROR: glBindVertexArray is NULL! Function not loaded.\n");
    exit(1);
  }
  glBindVertexArray(context->VAO); //initialize position buffer

  glBindBuffer(GL_ARRAY_BUFFER, context->VBO);

  int total_elements = context->numrows * context->numcols;
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * total_elements * 16, NULL, GL_DYNAMIC_DRAW);
  //Position buffer attribute assignment
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);//Position
  glEnableVertexAttribArray(0);

  //Texture buffer attribute assignment  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 4*sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1); //Texture

  //initialize texture buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * total_elements * 6, NULL, GL_DYNAMIC_DRAW);

  //initialize visibility buffer
  glBindBuffer(GL_ARRAY_BUFFER, context->VBO_visibility);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * total_elements * 4, NULL, GL_DYNAMIC_DRAW);

  //Visibility buffer attribute assignment
  glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(GLuint), (GLvoid*)0); 
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);


  context->vertices = malloc(sizeof(GLfloat) * total_elements * 16);
  context->indices = malloc(sizeof(GLuint) * total_elements * 6);
  context->visibility_data = malloc(sizeof(GLuint) * total_elements * 4); 
  context->visibleMapPrev = malloc(sizeof(char) * total_elements + 1);

  return 0;
}


//--------------------------------------
//          Update Map
//--------------------------------------

int update_map(char* map, graphicEnv* context){

  context->visibleMapPrev = strdup(map);


  memset(context->vertices, 0, sizeof(GLfloat) * context->total_elements * 16);
  memset(context->indices, 0, sizeof(GLuint) * context->total_elements * 6);
  memset(context->visibility_data, 0, sizeof(GLuint) * context->total_elements * 4);
  //init vars
  int numcols = context->numcols;
  int numrows = context->numrows;
  char elem;
  int total_elements = numrows * numcols;
  GLfloat* vertices = malloc(total_elements * sizeof(GLfloat) * 16);
  GLuint* indices = malloc(total_elements * sizeof(GLuint) * 6);
  GLuint* visibility_data = malloc(sizeof(GLuint) * total_elements * 16);
  if (!visibility_data) {
    fprintf(stderr, "malloc failed");
    exit(1);
  }
  int numel = 0;
  int vertices_ind = 0;
  int indices_ind = 0;
  int t_Idx[2] = {0,0};
  int i = 0;
  int j = numrows - 1;
  //loop to populate vertices & indices
  //fprintf(stdout, "entering population loop");
  while(*map != '\0'){
    elem = *map;
    //fprintf(stdout, "%c", elem);
    if(elem == '\n'){
      j--;
      i = 0;
      map++;
      continue;
    }

   

    get_textureIdx(elem, &t_Idx);
    // fprintf(stdout, "%c%d%d", elem, t_Idx[0], t_Idx[1]);
    // fflush(stdout);
    float numtext_perside = 6.0f; 
    //********Define Vertices***********
    GLfloat vertices_curr[16];
    square_vertices_from_info(numrows, numcols, numtext_perside, t_Idx[0], t_Idx[1], i, j, vertices_curr);
    //********Define Indices***********
    GLuint indices_curr[6] = {
      0 + 4*numel, 1 + 4*numel, 2 + 4*numel,  
      1 + 4*numel, 2 + 4*numel, 3 + 4*numel 
    };
    if(elem != ' '){

      for (int i = vertices_ind/4; i < vertices_ind/4 + 4; i++) {

        visibility_data[i] = 1;
      }
    }
    else{
      for (int i = vertices_ind/4; i < vertices_ind/4 + 4; i++) {

        visibility_data[i] = 0;
      }  
    }


    //check for overflow
    if (vertices_ind + 16 > total_elements * 16 || indices_ind + 6 > total_elements * 6) {
      // fprintf(stderr, "Buffer overflow detected during map parsing\n");
      break;
    }


    memcpy(indices + indices_ind, indices_curr, 6 * sizeof(GLuint));
    memcpy(vertices + vertices_ind, vertices_curr, 16 * sizeof(GLfloat));

    vertices_ind += 16;
    indices_ind += 6;
    numel++;
    i++;
    map++;
}
  context->num_vertices = vertices_ind;
  context->num_indices = indices_ind;

  // printf("First few vertex positions: ");
  //for (int i = 0; i < 16; i++) {
  // fprintf(stdout, "%f ", vertices[i]);
  //}
  //fprintf(stdout, "\n");

  glBindVertexArray(context->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices_ind, vertices);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->EBO);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * indices_ind, indices);

  glBindBuffer(GL_ARRAY_BUFFER, context->VBO_visibility);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLuint) * (vertices_ind / 4), visibility_data);

  // Unbind vertex array object
  glBindVertexArray(0);

  float identity_transform[9] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };

  glUseProgram(context->shaderProgram); 

  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, context->texture);

  glUniform1i(context->textureIdx, 0);

  glUniformMatrix3fv(context->transformIdx, 1, GL_FALSE, identity_transform);

  context->visibleMapPrev = strdup(map);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    fprintf(stderr, "OpenGL error: %d\n", err);
    return 1;
  }


  return 0;
}




//--------------------------------------
//          Initialize Context
//--------------------------------------
char* get_file_contents(const char* filename){
	FILE* file = fopen(filename, "r");
	int numchars = 0;
	while(fgetc(file) != EOF){
		numchars++;
	}
	rewind(file);
	char* contents = malloc(sizeof(char) * numchars + sizeof(char));
	char ch = '\0';
	int i = 0;
	while((ch = fgetc(file)) != EOF){
		contents[i] = ch;
		i++;
	}
	contents[i] = '\0';
	return contents;
}


int context_init(int numrows, int numcols, graphicEnv* context){

  //initialize glfw
  glfwInit();

  //set opengl version/profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //create window
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  if(monitor == NULL){

    fprintf(stderr, "failed to fetch monitor\n");
    return 1;
  }
  const GLFWvidmode* info = glfwGetVideoMode(monitor);
  if (info == NULL){

    fprintf(stderr, "failed to fetch monitor dimensions\n");
    return 1;
  }
  //determine aspect ratio of window to create
  float aspect_ratio = (float)context->numrows / (float)context->numcols;
  int maxdim = 1280;
  if(numrows > numcols){

    context->window = glfwCreateWindow((int)((float)maxdim*aspect_ratio), maxdim, "MyWindow", NULL, NULL);
    glfwSetWindowAspectRatio(context->window, (int)((float)maxdim*aspect_ratio), maxdim);
  }
  else{

    context->window = glfwCreateWindow((int)((float)maxdim), (int)((float)maxdim*aspect_ratio), "MyWindow", NULL, NULL);
    glfwSetWindowAspectRatio(context->window, (int)((float)maxdim), (int)((float)maxdim*aspect_ratio));
  }

  glfwSetWindowAttrib(context->window, GLFW_RESIZABLE, GLFW_TRUE);
  if (context->window == NULL){

    fprintf(stderr, "Failed to generate window");
    glfwTerminate();
    return 1;
  }
  //set the current context
  glfwMakeContextCurrent(context->window);

  //initialize the buffer to manage user input
    // typedef struct keys{
//   char[8] string;
//   char[8] truth;
// }keys_t;
 glfwSetInputMode(context->window, GLFW_STICKY_KEYS, GLFW_TRUE);
  keys_t* keys = malloc(sizeof(keys_t));
  if (keys == NULL){
    fprintf(stderr, "Memory allocation failed!");
    return 1;
  }
  memcpy(keys->string, "wasdqezcr", sizeof(keys->string));
  memset(keys->truth, 0, sizeof(keys->truth));
  glfwSetWindowUserPointer(context->window, (void*)keys);


  glfwSetFramebufferSizeCallback(context->window, framebuffer_size_callback);
  glfwSetKeyCallback(context->window, key_callback);
  //Load glad 
  gladLoadGL();

  int fb_width, fb_height;
  glfwGetFramebufferSize(context->window, &fb_width, &fb_height);
  framebuffer_size_callback(context->window, fb_width, fb_height);


  char* vertexShaderSource = get_file_contents("./Client/Graphics_Libraries/Shaders/default.vert");
  char* fragShaderSource = get_file_contents("./Client/Graphics_Libraries/Shaders/default.frag");


  if (vertexShaderSource == NULL || fragShaderSource == NULL) {

    fprintf(stderr, "Error: Failed to load shader files\n");
    free(vertexShaderSource);
    free(fragShaderSource);
    return 1;
  }

  const char* vertsrc = vertexShaderSource;
  const char* fragsrc = fragShaderSource;
  //fprintf(stdout, "printing vertsrc %s", vertsrc);
  fflush(stdout);
  //Initialize vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //creates reference
  glShaderSource(vertexShader, 1, &vertsrc, NULL); //specifies source
  glCompileShader(vertexShader); //compiles
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    fprintf(stdout, "Vertex shader compilation failed:\n%s\n", infoLog);

    return 1;
}



  //Initialize fragment shader
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER); //creates reference
  glShaderSource(fragShader, 1, &fragsrc, NULL); //specifies source
  glCompileShader(fragShader); //compiles
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
    fprintf(stdout, "Fragment shader compilation failed:\n%s\n", infoLog);
    return 1;
  }




  free(vertexShaderSource);
  free(fragShaderSource);

  //Create shader program
  context->shaderProgram = glCreateProgram(); //Creating a shader program and attaching the vertex and fragment shaders to a shaderProgram and linking to that program before deleting them
  glAttachShader(context->shaderProgram, vertexShader);
  glAttachShader(context->shaderProgram, fragShader);
  glLinkProgram(context->shaderProgram);

  glGetProgramiv(context->shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(context->shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "Shader program linking failed:\n%s\n", infoLog);
    return 1;
  }

  context->transformIdx = glGetUniformLocation(context->shaderProgram, "transform");


  context->textureIdx = glGetUniformLocation(context->shaderProgram, "aTexture");


  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);

  //*********Load Texture************
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load("./Client/Graphics_Libraries/textures/texture.png", &width, &height, &nrChannels, 0);


fprintf(stdout, "stbi error: %s\n", stbi_failure_reason());
fprintf(stdout, "data ptr: %p\n", (void*)data);
fprintf(stdout, "width: %d, height: %d, channels: %d\n", width, height, nrChannels);
fflush(stdout);


fprintf(stdout, "First byte: %d\n", data[0]);
fflush(stdout);

  printf("Generated VAO: %u\n", context->VAO);

  //fprintf(stdout, "Image dimensions: %dx%d channels: %d\n", width, height, nrChannels);
  //fflush(stdout);

  if (!data) {
    fprintf(stderr, "stb_image error: %s\n", stbi_failure_reason());
    return 1;    
  }

  glGenTextures(1, &context->texture);
  glBindTexture(GL_TEXTURE_2D, context->texture);

fprintf(stdout, "First byte: %d\n", data[0]);
fflush(stdout);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
fprintf(stdout, "First byte: %d\n", data[0]);
fflush(stdout);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

fprintf(stdout, "First byte: %d\n", data[0]);
fflush(stdout);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  glUseProgram(context->shaderProgram);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    fprintf(stderr, "OpenGL error: %d\n", err);
    return 1;
  }

  context->numrows = numrows;
  context->numcols = numcols;
  int total_elements = numrows * numcols;
  context->total_elements = total_elements;

  return 0;
}