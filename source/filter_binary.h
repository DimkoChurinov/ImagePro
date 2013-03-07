#pragma once

#include "filter.h"

class QImage;
struct FilterInversion : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
};
struct FilterDilatation : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
};
struct FilterErosion : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
};