#include <logger.hpp>
#include <GL/glew.h>
#include <rendering/shaders.hpp>
#pragma region embedded shaders
const char* Resources::Shaders::VertexShader = 
"#version 330 core\n"

"// Input vertex data, different for all executions of this shader.\n"
"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
"layout(location = 1) in vec2 vertexUV;\n"
"layout(location = 2) in vec3 vertexNormal_modelspace;\n"
"layout(location = 3) in vec3 VertexColor_;\n"
"layout(location = 4) in float faceId;\n"

"// Output data ; will be interpolated for each fragment.\n"
"out vec2 UV;\n"
"out vec3 Position_worldspace;\n"
"out vec3 Normal_cameraspace;\n"
"out vec3 EyeDirection_cameraspace;\n"
"out vec3 LightDirection_cameraspace;\n"
"out vec3 VertexColor;\n"
"out float FaceID;\n"

"// Values that stay constant for the whole mesh.\n"
"uniform mat4 MVP;\n"
"uniform mat4 V;\n"
"uniform mat4 M;\n"
"uniform vec3 LightPosition_worldspace;\n"

"void main(){\n"
  "VertexColor = VertexColor_;\n"
  "FaceID = faceId;\n"

	"// Output position of the vertex, in clip space : MVP * position\n"
	"gl_Position =  MVP * vec4(vertexPosition_modelspace,1);\n"
	
	"// Position of the vertex, in worldspace : M * position\n"
	"Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;\n"
	
	"// Vector that goes from the vertex to the camera, in camera space.\n"
	"// In camera space, the camera is at the origin (0,0,0).\n"
	"vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;\n"
	"EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n"

	"// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.\n"
	"vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;\n"
	"LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;\n"
	
	"// Normal of the the vertex, in camera space\n"
	"Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.\n"
	
	"// UV of the vertex. No special space for this one.\n"
	"UV = vertexUV;\n"
"}";


const char* Resources::Shaders::FragmentShader = "#version 330 core\n"

"in vec2 UV;\n"
"in vec3 Position_worldspace;\n"
"in vec3 Normal_cameraspace;\n"
"in vec3 EyeDirection_cameraspace;\n"
"in vec3 LightDirection_cameraspace;\n"
"in vec3 VertexColor;\n"
"in float FaceID;\n"

"// Ouput data\n"
"layout(location = 0) out vec4 color;\n"

"// Values that stay constant for the whole mesh.\n"
"uniform sampler2D myTextureSampler;\n"
"uniform mat4 MV;\n"
"uniform float Selected;\n"
"uniform vec3 LightPosition_worldspace;\n"
"// Values that stay constant for the whole mesh.\n"

"void main(){\n"
	"vec3 LightColor = vec3(1,1,1);\n"
	"float LightPower = 50.0f;\n"
	"// Material properties\n"
	"vec3 MaterialDiffuseColor = VertexColor;\n"
	"if(FaceID == Selected) MaterialDiffuseColor = vec3(1,1,1);\n"
	"vec3 MaterialAmbientColor = vec3(0.5,0.5,0.5) * MaterialDiffuseColor;\n"
	"vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);\n"
	"// Distance to the light\n"
	"float distance = length( LightPosition_worldspace - Position_worldspace );\n"
	"// Normal of the computed fragment, in camera space\n"
	"vec3 n = normalize( Normal_cameraspace );\n"
	"// Direction of the light (from the fragment to the light)\n"
	"vec3 l = normalize( LightDirection_cameraspace );\n"
	"// Cosine of the angle between the normal and the light direction, \n"
	"// clamped above 0\n"
	"//  - light is at the vertical of the triangle -> 1\n"
	"//  - light is perpendicular to the triangle -> 0\n"
	"//  - light is behind the triangle -> 0\n"
	"float cosTheta = clamp( dot( n,l ), 0,1 );\n"
	"// Eye vector (towards the camera)\n"
	"vec3 E = normalize(EyeDirection_cameraspace);\n"
	"// Direction in which the triangle reflects the light\n"
	"vec3 R = reflect(-l,n);\n"
	"// Cosine of the angle between the Eye vector and the Reflect vector,\n"
	"// clamped to 0\n"
	"//  - Looking into the reflection -> 1\n"
	"//  - Looking elsewhere -> < 1\n"
	"float cosAlpha = clamp( dot( E,R ), 0,1 );\n"
	"vec3 pre_color = \n"
	"// Ambient : simulates indirect lighting\n"
	"MaterialAmbientColor +\n"
	"// Diffuse : color of the object\n"
	"MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +\n"
	"// Specular : reflective highlight, like a mirror\n"
	"MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);\n"
  "color = vec4(pre_color,1);\n"
  
"}";
#pragma endregion

GLuint Resources::Shaders::CompileShaders(const char *vertex, const char *fragment){
	GLint log_length, success;
  GLuint fragment_shader, program, vertex_shader;

  /* Vertex shader */
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex, NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    Logger::Error("Vertex shader compile error:\n");
    std::vector<char> VertError(log_length+1);
    glGetShaderInfoLog(vertex_shader, log_length, NULL, &VertError[0]);
    Logger::Error("%s\n", &VertError[0]);
  }
  if (!success) {
    printf("vertex shader compile error\n");
    exit(EXIT_FAILURE);
  }

  /* Fragment shader */
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    Logger::Error("Fragment shader compile error:\n");
    std::vector<char> FragError(log_length+1);
    glGetShaderInfoLog(fragment_shader, log_length, NULL, &FragError[0]);
    Logger::Error("%s\n", &FragError[0]);
  }
  if (!success) {
    Logger::Critical("fragment shader compile error\n");
    exit(EXIT_FAILURE);
  }

  /* Link shaders */
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    Logger::Error("Shader linking error:\n");
    std::vector<char> ProgError(log_length+1);
    glGetProgramInfoLog(program, log_length, NULL, &ProgError[0]);
    Logger::Error("%s\n", &ProgError[0]);
  }
  if (!success) {
    printf("shader link error");
    exit(EXIT_FAILURE);
  }
  else{}
  Logger::Info("[readmdl shaders] linked program\n");

  /* Cleanup. */
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return program;
}