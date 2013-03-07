#pragma once

class QImage;
class QWidget;
struct Filter {
  virtual void Apply(const QImage &imageSrc, QImage &imageDest) const = 0;
  virtual bool ShowParamsDialog(QWidget *parent) { return true; }
};

inline int clamp(int x, int minVal, int maxVal)
{
  return x < minVal ? minVal : (x > maxVal ? maxVal : x);
}
