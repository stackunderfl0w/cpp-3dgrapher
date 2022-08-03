//
// Created by Pat on 4/4/2020.
//

#ifndef INC_3DGRAPHER_SHADERS_H
#define INC_3DGRAPHER_SHADERS_H


const char* simplevertextransform =
"  																		\n"
"	// Input vertex data, different for all executions of this shader.	\n"
"	attribute vec3 vertexPosition_modelspace;							\n"
"	// Values that stay constant for the whole mesh.					\n"
"	uniform mat4 MVP;													\n"
"	void main(){														\n"
"  // Output position of the vertex, in clip space : MVP * position		\n"
"  		gl_Position =  MVP * vec4(vertexPosition_modelspace,1);			\n"
"	}																	\n"
    "\n";

const char* singlecolorfragment =
"precision mediump float;\n"
"   								\n"
"    void main(){					\n"
"      gl_FragColor = vec4(1,0,0,1);			\n"
"    }								\n";

const char* basiclightingvertextransform =
"															\n"
"attribute vec3 aPos;										\n"
"attribute vec3 aNormal;									\n"
"															\n"
"varying vec3 FragPos;											\n"
"varying vec3 Normal;											\n"
"															\n"
"uniform mat4 model;										\n"
"uniform mat4 view;											\n"
"uniform mat4 projection;									\n"

"void main()												\n"
"{															\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));				\n"
"    //Normal = mat3(transpose(inverse(model))) * aNormal;	\n"
"    Normal=aNormal;										\n"
"															\n"
"    gl_Position = projection * view * vec4(FragPos, 1.0);	\n"
"}															\n";

const char* basiclightingfragment =
"precision highp  float;											\n"
"varying vec3 Normal;													\n"
"varying vec3 FragPos;													\n"

"uniform vec3 lightPos;												\n"
"uniform vec3 viewPos;												\n"
"uniform vec3 lightColor;											\n"
"uniform vec3 objectColor;											\n"

"void main()														\n"
"{																	\n"
"    // ambient														\n"
"    float ambientStrength = 0.1;									\n"
"    vec3 ambient = ambientStrength * lightColor;					\n"
"																	\n"
"    // diffuse														\n"
"    vec3 norm = normalize(Normal);									\n"
"    vec3 lightDir = normalize(lightPos - FragPos);					\n"
"    float diff = max(dot(norm, lightDir), 0.0);					\n"
"    vec3 diffuse = diff * lightColor;								\n"
"																	\n"
"    // specular													\n"
"    float specularStrength = 0.5;									\n"
"    vec3 viewDir = normalize(viewPos - FragPos);					\n"
"    vec3 reflectDir = reflect(-lightDir, norm);					\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);	\n"
"    vec3 specular = specularStrength * spec * lightColor;			\n"
"																	\n"
"    vec3 result = (ambient + diffuse + specular) * objectColor;	\n"
"    gl_FragColor = vec4(result.rgb, 1.0);								\n"
"}																	\n"
" 																	\n";

#endif //INC_3DGRAPHER_SHADERS_H
