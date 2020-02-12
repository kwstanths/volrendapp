// Author: Marc Comino 2019

#include <volume_io.h>

#include <boost/filesystem.hpp>

#include <QImage>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "./volume.h"

namespace data_representation {

namespace {

bool compare(const boost::filesystem::path& a,
             const boost::filesystem::path& b) {
  if (a.size() == b.size())
    return a < b;
  else
    return a.size() < b.size();
}

}  // namespace

bool ReadFromDicom(const std::string& path, Volume* vol) {
  const boost::filesystem::path kDir = boost::filesystem::path(path);

  if (!boost::filesystem::exists(kDir) ||
      !boost::filesystem::is_directory(kDir))
    return false;
  vol->histogram_.clear();
  vol->histogram_.resize(256, 0);

  std::vector<boost::filesystem::path> paths(
      boost::filesystem::directory_iterator{kDir},
      boost::filesystem::directory_iterator{});
  std::sort(paths.begin(), paths.end(), compare);

  vol->depth_ = 0;
  std::vector<uchar> data;
  for (auto const& file_path : paths) {
    if (boost::filesystem::is_regular_file(file_path) &&
        file_path.extension() == ".jpg") {
      std::cout << file_path.string() << std::endl;

      QImage img(QString::fromStdString(file_path.string()));

      if (img.isNull()) return false;

      if (vol->depth_ == 0) {
        vol->width_ = img.width();
        vol->height_ = img.height();
      } else if (vol->width_ != img.width() || vol->height_ != img.height()) {
        return false;
      }

      vol->depth_++;

      for (int i = 0; i < vol->width_; ++i) {
        for (int j = 0; j < vol->height_; ++j) {
          data.push_back(img.pixel(j, i) & 0xFF);
        }
      }
    }
  }

  const int kDataSize = data.size();

  // Generate the 3D texture.
  for (int i = 0; i < kDataSize; i++) {
    vol->histogram_[static_cast<int>(data[i])] += 1.0;
  }

  glGenTextures(1, &vol->id_);
  glBindTexture(GL_TEXTURE_3D, vol->id_);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, vol->width_, vol->height_, vol->depth_,
               0, GL_RED, GL_UNSIGNED_BYTE, &data[0]);
  glGenerateMipmap(GL_TEXTURE_3D);

  std::vector<double> sorted_histogram_;
  sorted_histogram_.insert(sorted_histogram_.begin(), vol->histogram_.begin(),
                           vol->histogram_.end());
  sort(sorted_histogram_.begin(), sorted_histogram_.end());

  const double kMaximum = sorted_histogram_[sorted_histogram_.size() * 0.98];

  const int kHistSize = vol->histogram_.size();
  for (int i = 0; i < kHistSize; ++i) {
    vol->histogram_[i] = vol->histogram_[i] / kMaximum;
  }

  std::cout << "Volume loaded, 3D texture built: " << vol->width_ << " x "
            << vol->height_ << " x " << vol->depth_ << std::endl;

  return true;
}

}  // namespace data_representation
