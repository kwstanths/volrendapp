// Author: Marc Comino 2019

#include <volume.h>

#include <algorithm>
#include <limits>

namespace data_representation {

Volume::Volume() : width_(0), height_(0), depth_(0), id_(0) {}

Volume::~Volume() { Clear(); }

void Volume::Clear() {
  histogram_.clear();
  width_ = 0;
  height_ = 0;
  depth_ = 0;
  id_ = 0;
}

GLuint Volume::GetTextureId() { return id_; }

}  // namespace data_representation
