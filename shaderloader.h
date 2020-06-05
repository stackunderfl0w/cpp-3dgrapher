//
// Created by Pat on 4/4/2020.
//

#ifndef INC_3DGRAPHER_SHADERLOADER_H
#define INC_3DGRAPHER_SHADERLOADER_H


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint LoadShaders_text(const char * vertex_source,const char * fragment_source);


#endif //INC_3DGRAPHER_SHADERLOADER_H
