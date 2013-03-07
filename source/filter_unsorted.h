#pragma once
#include <QColor>
#include "qglobal.h"
#include "filter.h"

struct FilterWhiteNoise : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  double p;
  uchar d;
};

struct FilterDust : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  double p;
  uchar min;
};
struct FilterGrid : Filter {
//  FilterGrid(unsigned w, unsigned h);
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  unsigned m_wStep;
  unsigned m_hStep;
  QColor   m_color;
};