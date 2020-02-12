// Author: Marc Comino 2018

#include <cube.h>

#include <algorithm>
#include <limits>

namespace data_representation {

Cube::Cube()
    : min_(Eigen::Vector3f(-0.5f, -0.5f, -0.5f)),
      max_(Eigen::Vector3f(0.5f, 0.5f, 0.5f)),
      element_count_(36) {
  std::vector<float> vertices = {-0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f,
                                 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,
                                 -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,
                                 0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f};

  glGenBuffers(1, &vbo_id_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0],
               GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao_id_);
  glBindVertexArray(vao_id_);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  std::vector<int> faces = {0, 5, 4, 5, 0, 1, 3, 7, 6, 3, 6, 2,
                            7, 4, 6, 6, 4, 5, 2, 1, 3, 3, 1, 0,
                            3, 0, 7, 7, 0, 4, 6, 5, 2, 2, 5, 1};

  glGenBuffers(1, &faces_id_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces_id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faces.size(),
               &faces[0], GL_STATIC_DRAW);
}

Cube::~Cube() {
  glDeleteBuffers(1, &vbo_id_);
  glDeleteVertexArrays(1, &vao_id_);
  glDeleteBuffers(1, &faces_id_);
}

void Cube::Render() {
  glBindVertexArray(vao_id_);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces_id_);
  glDrawRangeElements(GL_TRIANGLES, 0, element_count_ - 1, element_count_,
                      GL_UNSIGNED_INT, reinterpret_cast<GLvoid *>(0));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

}  // namespace data_representation
