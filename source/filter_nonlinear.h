#pragma once

#include <QImage>
#include "filter.h"
#include "qglobal.h"

struct FilterMedian : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  uchar radius;
};