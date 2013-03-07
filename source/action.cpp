#include "action.h"
#include "filter.h"


void ActionApplyFilter::Redo (QImage &image)
{
  if (m_resultValid) {
    image = m_resultImage;
    return;
  }
  // copy image for undo
  m_savedImageCopy = image;
  m_filter.Apply(m_savedImageCopy, image);
  m_resultImage = image;
  m_resultValid = true;
}

void ActionApplyFilter::Undo (QImage &image)
{
  image = m_savedImageCopy;
}

bool ActionApplyFilter::PrepareFilterParams (QWidget *parent)
{
  return m_filter.ShowParamsDialog(parent);
}