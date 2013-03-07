#pragma once

#include <QImage>

class Action {
public:
  virtual void Redo(QImage &image) = 0;
  virtual void Undo(QImage &image) = 0;
};

class QWidget;
struct Filter;
class ActionApplyFilter : public Action {
public:
  ActionApplyFilter(Filter &filter) : m_filter(filter), m_resultValid(false) {}

  virtual void Redo(QImage &image);
  virtual void Undo(QImage &image);

  bool PrepareFilterParams(QWidget *parent);

private:
  QImage  m_savedImageCopy,
          m_resultImage;
  bool    m_resultValid;
  Filter &m_filter;
};
