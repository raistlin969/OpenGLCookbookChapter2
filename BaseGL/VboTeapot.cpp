#include "vboteapot.h"
#include "teapotdata.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <cstdio>

#include <glm/gtc/matrix_transform.hpp>
using glm::mat4;
using glm::vec4;

VBOTeapot::VBOTeapot(int grid, mat4 lidTransform)
{
    int verts = 32 * (grid + 1) * (grid + 1);
    _faces = grid * grid * 32;
    float * v = new float[ verts * 3 ];
    float * n = new float[ verts * 3 ];
    float * tc = new float[ verts * 2 ];
    unsigned int * el = new unsigned int[_faces * 6];

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray(_vao);

    unsigned int handle[4];
    glGenBuffers(4, handle);

    GeneratePatches( v, n, tc, el, grid );
    MoveLid(grid, v, lidTransform);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), v, GL_STATIC_DRAW);
    glVertexAttribPointer( (GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, (3 * verts) * sizeof(float), n, GL_STATIC_DRAW);
    glVertexAttribPointer( (GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(1);  // Vertex normal

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, (2 * verts) * sizeof(float), tc, GL_STATIC_DRAW);
    glVertexAttribPointer( (GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(2);  // texture coords

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * _faces * sizeof(unsigned int), el, GL_STATIC_DRAW);

    delete [] v;
    delete [] n;
    delete [] el;
    delete [] tc;

    glBindVertexArray(0);
}

void VBOTeapot::GeneratePatches(float * v, float * n, float * tc, unsigned int* el, int grid) {
    float * B = new float[4*(grid+1)];  // Pre-computed Bernstein basis functions
    float * dB = new float[4*(grid+1)]; // Pre-computed derivitives of basis functions

    int idx = 0, elIndex = 0, tcIndex = 0;

    // Pre-compute the basis functions  (Bernstein polynomials)
    // and their derivatives
    ComputeBasisFunctions(B, dB, grid);

    // Build each patch
    // The rim
    BuildPatchReflect(0, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The body
    BuildPatchReflect(1, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    BuildPatchReflect(2, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The lid
    BuildPatchReflect(3, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    BuildPatchReflect(4, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The bottom
    BuildPatchReflect(5, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The handle
    BuildPatchReflect(6, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
    BuildPatchReflect(7, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
    // The spout
    BuildPatchReflect(8, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
    BuildPatchReflect(9, B, dB, v, n, tc, el, idx, elIndex, tcIndex, grid, false, true);

    delete [] B;
    delete [] dB;
}

void VBOTeapot::MoveLid(int grid, float *v, mat4 lidTransform) {

    int start = 3 * 12 * (grid+1) * (grid+1);
    int end = 3 * 20 * (grid+1) * (grid+1);

    for( int i = start; i < end; i+=3 )
    {
        vec4 vert = vec4(v[i], v[i+1], v[i+2], 1.0f );
        vert = lidTransform * vert;
        v[i] = vert.x;
        v[i+1] = vert.y;
        v[i+2] = vert.z;
    }
}

void VBOTeapot::BuildPatchReflect(int patchNum,
                                    float *B, float *dB,
                                    float *v, float *n,
                                    float *tc, unsigned int *el,
                                    int &index, int &elIndex, int &tcIndex, int grid,
                                    bool reflectX, bool reflectY)
{
    vec3 patch[4][4];
    vec3 patchRevV[4][4];
    GetPatch(patchNum, patch, false);
    GetPatch(patchNum, patchRevV, true);

    // Patch without modification
    BuildPatch(patch, B, dB, v, n, tc, el,
               index, elIndex, tcIndex, grid, mat3(1.0f), true);

    // Patch reflected in x
    if( reflectX ) {
        BuildPatch(patchRevV, B, dB, v, n, tc, el,
                   index, elIndex, tcIndex, grid, mat3(vec3(-1.0f, 0.0f, 0.0f),
                                              vec3(0.0f, 1.0f, 0.0f),
                                              vec3(0.0f, 0.0f, 1.0f) ), false );
    }

    // Patch reflected in y
    if( reflectY ) {
        BuildPatch(patchRevV, B, dB, v, n, tc, el,
                   index, elIndex, tcIndex, grid, mat3(vec3(1.0f, 0.0f, 0.0f),
                                              vec3(0.0f, -1.0f, 0.0f),
                                              vec3(0.0f, 0.0f, 1.0f) ), false );
    }

    // Patch reflected in x and y
    if( reflectX && reflectY ) {
        BuildPatch(patch, B, dB, v, n, tc, el,
                   index, elIndex, tcIndex, grid, mat3(vec3(-1.0f, 0.0f, 0.0f),
                                              vec3(0.0f, -1.0f, 0.0f),
                                              vec3(0.0f, 0.0f, 1.0f) ), true );
    }
}

void VBOTeapot::BuildPatch(vec3 patch[][4],
                           float *B, float *dB,
                           float *v, float *n, float *tc,
                           unsigned int *el,
                           int &index, int &elIndex, int &tcIndex, int grid, mat3 reflect,
                           bool invertNormal)
{
    int startIndex = index / 3;
    float tcFactor = 1.0f / grid;

    for( int i = 0; i <= grid; i++ )
    {
        for( int j = 0 ; j <= grid; j++)
        {
            vec3 pt = reflect * Evaluate(i,j,B,patch);
            vec3 norm = reflect * EvaluateNormal(i,j,B,dB,patch);
            if( invertNormal )
                norm = -norm;

            v[index] = pt.x;
            v[index+1] = pt.y;
            v[index+2] = pt.z;

            n[index] = norm.x;
            n[index+1] = norm.y;
            n[index+2] = norm.z;

            tc[tcIndex] = i * tcFactor;
            tc[tcIndex+1] = j * tcFactor;

            index += 3;
            tcIndex += 2;
        }
    }

    for( int i = 0; i < grid; i++ )
    {
        int iStart = i * (grid+1) + startIndex;
        int nextiStart = (i+1) * (grid+1) + startIndex;
        for( int j = 0; j < grid; j++)
        {
            el[elIndex] = iStart + j;
            el[elIndex+1] = nextiStart + j + 1;
            el[elIndex+2] = nextiStart + j;

            el[elIndex+3] = iStart + j;
            el[elIndex+4] = iStart + j + 1;
            el[elIndex+5] = nextiStart + j + 1;

            elIndex += 6;
        }
    }
}

void VBOTeapot::GetPatch( int patchNum, vec3 patch[][4], bool reverseV )
{
    for( int u = 0; u < 4; u++) {          // Loop in u direction
        for( int v = 0; v < 4; v++ ) {     // Loop in v direction
            if( reverseV ) {
                patch[u][v] = vec3(
                        Teapot::cpdata[Teapot::patchdata[patchNum][u*4+(3-v)]][0],
                        Teapot::cpdata[Teapot::patchdata[patchNum][u*4+(3-v)]][1],
                        Teapot::cpdata[Teapot::patchdata[patchNum][u*4+(3-v)]][2]
                        );
            } else {
                patch[u][v] = vec3(
                        Teapot::cpdata[Teapot::patchdata[patchNum][u*4+v]][0],
                        Teapot::cpdata[Teapot::patchdata[patchNum][u*4+v]][1],
                        Teapot::cpdata[Teapot::patchdata[patchNum][u*4+v]][2]
                        );
            }
        }
    }
}

void VBOTeapot::ComputeBasisFunctions( float * B, float * dB, int grid ) {
    float inc = 1.0f / grid;
    for( int i = 0; i <= grid; i++ )
    {
        float t = i * inc;
        float tSqr = t * t;
        float oneMinusT = (1.0f - t);
        float oneMinusT2 = oneMinusT * oneMinusT;

        B[i*4 + 0] = oneMinusT * oneMinusT2;
        B[i*4 + 1] = 3.0f * oneMinusT2 * t;
        B[i*4 + 2] = 3.0f * oneMinusT * tSqr;
        B[i*4 + 3] = t * tSqr;

        dB[i*4 + 0] = -3.0f * oneMinusT2;
        dB[i*4 + 1] = -6.0f * t * oneMinusT + 3.0f * oneMinusT2;
        dB[i*4 + 2] = -3.0f * tSqr + 6.0f * t * oneMinusT;
        dB[i*4 + 3] = 3.0f * tSqr;
    }
}


vec3 VBOTeapot::Evaluate( int gridU, int gridV, float *B, vec3 patch[][4] )
{
    vec3 p(0.0f,0.0f,0.0f);
    for( int i = 0; i < 4; i++) {
        for( int j = 0; j < 4; j++) {
            p += patch[i][j] * B[gridU*4+i] * B[gridV*4+j];
        }
    }
    return p;
}

vec3 VBOTeapot::EvaluateNormal( int gridU, int gridV, float *B, float *dB, vec3 patch[][4] )
{
    vec3 du(0.0f,0.0f,0.0f);
    vec3 dv(0.0f,0.0f,0.0f);

    for( int i = 0; i < 4; i++) {
        for( int j = 0; j < 4; j++) {
            du += patch[i][j] * dB[gridU*4+i] * B[gridV*4+j];
            dv += patch[i][j] * B[gridU*4+i] * dB[gridV*4+j];
        }
    }
    return glm::normalize( glm::cross( du, dv ) );
}

void VBOTeapot::Render()
{
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, 6 * _faces, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
}
