#pragma once

#ifndef CubeValues_h__
#define CubeValues_h__

// Some defines to make calculations below more transparent
const int TRIANGLES_PER_FACE = 2;
const int VERTICES_PER_TRIANGLE = 3;
const int VERTICES_PER_EDGE = 2;
const int FLOATS_PER_VERTEX = 3;

// Cube geometry
const int VERT_COUNT = 8;
const int FACE_COUNT = 6;
const int EDGE_COUNT = 12;

const float CUBE_SIZE = 0.4f;
const float CUBE_P = (CUBE_SIZE / 2.0f);
const float CUBE_N = (-1.0f * CUBE_P);

const float ON = 1.0;
const float PQ = 0.25;

#define RED 1, 0, 0
#define GREEN 0, 1, 0
#define BLUE 0, 0, 1
#define YELLOW 1, 1, 0
#define CYAN 0, 1, 1
#define MAGENTA 1, 0, 1

// How big do we want our renderbuffer
const int FRAMEBUFFER_OBJECT_SCALE = 3;

const glm::vec3 X_AXIS = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 Y_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);        // Animate the cube
const glm::vec3 CAMERA = glm::vec3(0.0f, 0.0f, 0.8f);
const glm::vec3 ORIGIN = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 UP = Y_AXIS;

// Vertices for a unit cube centered at the origin
const GLfloat CUBE_VERTEX_DATA[VERT_COUNT * FLOATS_PER_VERTEX] = {
	CUBE_N, CUBE_N, CUBE_N, // Vertex 0 position
	CUBE_P, CUBE_N, CUBE_N, // Vertex 1 position
	CUBE_P, CUBE_P, CUBE_N, // Vertex 2 position
	CUBE_N, CUBE_P, CUBE_N, // Vertex 3 position
	CUBE_N, CUBE_N, CUBE_P, // Vertex 4 position
	CUBE_P, CUBE_N, CUBE_P, // Vertex 5 position
	CUBE_P, CUBE_P, CUBE_P, // Vertex 6 position
	CUBE_N, CUBE_P, CUBE_P, // Vertex 7 position
};


const GLfloat CUBE_FACE_COLORS[] = {
	RED, 1,
	GREEN, 1,
	BLUE, 1,
	YELLOW, 1,
	CYAN, 1,
	MAGENTA, 1,
};

// 6 sides * 2 triangles * 3 vertices
const unsigned int CUBE_INDICES[FACE_COUNT * TRIANGLES_PER_FACE * VERTICES_PER_TRIANGLE ] = {
   0, 4, 5, 0, 5, 1, // Face 0
   1, 5, 6, 1, 6, 2, // Face 1
   2, 6, 7, 2, 7, 3, // Face 2
   3, 7, 4, 3, 4, 0, // Face 3
   4, 7, 6, 4, 6, 5, // Face 4
   3, 0, 1, 3, 1, 2  // Face 5
};

//
const unsigned int CUBE_WIRE_INDICES[EDGE_COUNT * VERTICES_PER_EDGE ] = {
   0, 1, 1, 2, 2, 3, 3, 0, // square
   4, 5, 5, 6, 6, 7, 7, 4, // facing square
   0, 4, 1, 5, 2, 6, 3, 7, // transverse lines
};

const GLfloat QUAD_VERTICES[] = {
	-1, -1, 0, 0,
	 1, -1, 1, 0,
	 1,  1, 1, 1,
	-1,  1, 0, 1,
};

const GLuint QUAD_INDICES[] = {
   2, 0, 3, 0, 1, 2,
};

#endif // CubeValues_h__