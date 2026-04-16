#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "./Graphics_Libraries/glad/include/glad/glad.h" 
#include "./Graphics_Libraries/glfw-3.4/include/GLFW/glfw3.h"

//---------------------------------------------------
//                    GRAPHICS
//---------------------------------------------------
/*
 * Graphics Environment Struct
 */
typedef struct {
  GLFWwindow* window;
  GLuint shaderProgram;
  GLuint texture;
  GLuint VAO, VBO, VBO_visibility, EBO, transformIdx, textureIdx;
  GLuint num_vertices;
  GLuint num_indices;
  int numrows;
  int numcols;
  int total_elements;
  char* visibleMapPrev;
  GLfloat* vertices;
  GLuint* indices;
  GLuint* visibility_data;
} graphicEnv;



//---------------------------------------------------
//                    INPUT HANDLING
//---------------------------------------------------
#define MAX_KEYS 9
typedef struct keys{
  char string[MAX_KEYS];
  char truth[MAX_KEYS];
}keys_t;



int context_init(int numrows, int numcols, graphicEnv* context);
int graphics_loop(graphicEnv* context);
int update_map(char* map, graphicEnv* context);
void square_vertices_from_info(int numcols, int numrows, int numtext_perside, int texture_xindex, int texture_yindex, int xindex, int yindex, float* vertices_out);
void get_textureIdx(char ch, int (*index)[2]);
int init_map_buffers(graphicEnv* context);
void free_map_buffers(graphicEnv* context);

