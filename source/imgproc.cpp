#include <QDir>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QEvent.h>
#include <math.h>
#include "imgproc.h"
#include "settings_dialog.h"
#include "image_comparator.h"

char  const *c_appName = "Team A: Paramonov, Churinov";
double  const c_ZoomInFactor = 1.25;
double const c_minZoomFactor = 0.25;
double const c_maxZoomFactor = 4.00;

inline double clamp(double x, double minVal, double maxVal)
{
  return x < minVal ? minVal : (x > maxVal ? maxVal : x);
}

imgproc::imgproc(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
  ui.setupUi(this);
  m_scrollArea = new QScrollArea;
  m_scrollArea->setBackgroundRole(QPalette::Dark);
  m_scrollArea->setWidget(ui.imageLabel);
  setCentralWidget(m_scrollArea);  
  setWindowTitle(tr(c_appName)); 
  ui.imageLabel->setScaledContents(true);
  ui.actionSave->setEnabled(false);
  ui.actionRedo->setEnabled(false);
  ui.actionUndo->setEnabled(false);
  ui.actionCompare->setEnabled(false);

  loadFilters();
  createActionsUI(ui);

  connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));
  connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(save()));
  connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui.actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
  connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
  connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
  connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
  connect(ui.actionCompare, SIGNAL(triggered()), this, SLOT(compare()));
  connect(&m_undoManager, SIGNAL(historyChanged()), this,  SLOT(onHistoryChanged()));
}

imgproc::~imgproc()
{

}


void imgproc::updateActions()
{  
}

void imgproc::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value()
    + ((factor - 1) * scrollBar->pageStep()/2)));
}

void imgproc::open()
{
  QString fileName = QFileDialog::getOpenFileName(this,
    tr("Open File"), QDir::currentPath(), tr("Images (*.png *.bmp *.jpg)"));
  if (fileName.isEmpty())
    return;
  QImage image(fileName);    
  if (image.isNull()) 
  {
    QMessageBox::information(this, tr(c_appName),
      tr("Could not load %1.").arg(fileName));
    return;
  }
  m_currFilePath = fileName;
  setWindowTitle(QFileInfo(fileName).fileName() + " - " + c_appName);
  image = image.convertToFormat(QImage::Format_ARGB32);
  m_originalPixmap = QPixmap::fromImage(image);
  m_undoManager.clear();
  ui.actionSave->setEnabled(true);
  ui.actionCompare->setEnabled(true);
  ui.imageLabel->setPixmap(QPixmap::fromImage(image));
  m_scaleFactor = 1.0;

  ui.actionFit_to_window->setEnabled(true);
  updateActions();

  for (int i = 0; i < m_filterActions.size(); i++) {
    m_filterActions[i]->setEnabled(true);
  }

  if (!ui.actionFit_to_window->isChecked())
    ui.imageLabel->adjustSize();
  
}

void imgproc::save()
{
  QString selected;
  QString fileName = QFileDialog::getSaveFileName(this,
    tr("Save File"), QDir::currentPath(), tr("PNG(*.png);;BMP(*.bmp);;JPG(*.jpg)"), &selected);
  if (fileName.isEmpty())
    return;
  QFileInfo fi(fileName);
  char const*ext;
  unsigned const c_extCount = 3;
  char const *availExt[c_extCount] = {"png", "bmp", "jpg"};
  for (unsigned i = 0; i < c_extCount; ++i)
  {
    if (selected.indexOf(availExt[i]) != -1)
    {
      ext = availExt[i];
      break;
    }
  }
  if(!m_originalPixmap.save(fileName, ext))
  {
    QMessageBox::information(this, tr(c_appName),
      tr("Could not save load %1.").arg(fileName));
    return;
  }
  m_currFilePath = fileName;
  setWindowTitle(QFileInfo(fileName).fileName() + " - " + c_appName);
}


void imgproc::redo ()
{
  if (m_undoManager.canRedo()) {
    QImage image = m_originalPixmap.toImage();
    m_undoManager.redo(image);
    m_originalPixmap = QPixmap::fromImage(image);
    ui.imageLabel->setPixmap(m_originalPixmap);
  }
}

void imgproc::undo ()
{
  if (m_undoManager.canUndo()) {
    QImage image = m_originalPixmap.toImage();
    m_undoManager.undo(image);
    m_originalPixmap = QPixmap::fromImage(image);
    ui.imageLabel->setPixmap(m_originalPixmap);
  }
}

void imgproc::onHistoryChanged ()
{
  ui.actionRedo->setEnabled(m_undoManager.canRedo());
  ui.actionUndo->setEnabled(m_undoManager.canUndo());
}

void imgproc::createActionsUI (Ui::imgprocClass &ui)
{
  // binary
  QAction *action = NULL;
  action = ui.menuBinary->addAction("Invert");
  m_filterActions.push_back(action);
  action = ui.menuBinary->addAction("Erosion");
  m_filterActions.push_back(action);
  action = ui.menuBinary->addAction("Dilatation");
  m_filterActions.push_back(action);
  
  // linear
  action = ui.menuLinear->addAction("Linear 3x3");
  m_filterActions.push_back(action);
  action = ui.menuLinear->addAction("Linear 5x5");
  m_filterActions.push_back(action);

  // common
  action = ui.menu_Filters->addAction("White Noise");
  m_filterActions.push_back(action);
  action = ui.menu_Filters->addAction("Dust");
  m_filterActions.push_back(action);
  action = ui.menu_Filters->addAction("Grid");
  m_filterActions.push_back(action);
  action = ui.menu_Filters->addAction("Median");
  m_filterActions.push_back(action);
 
  for (int i = 0; i < m_filterActions.size(); i++) {
    m_filterActions[i]->setEnabled(false);
    connect(m_filterActions[i], SIGNAL(triggered()), this, SLOT(onFilterAction()));
  }
  
}

#include "filter_binary.h"
#include "filter_unsorted.h"
#include "filter_linear.h"
#include "filter_nonlinear.h"

void imgproc::loadFilters ()
{
  m_filters["Invert"] = new FilterInversion();
  m_filters["Erosion"] = new FilterErosion();
  m_filters["Dilatation"] = new FilterDilatation();
  m_filters["White Noise"] = new FilterWhiteNoise();
  m_filters["Dust"] = new FilterDust();
  m_filters["Grid"] = new FilterGrid();
  m_filters["Linear 3x3"] = new FilterLinear3x3();
  m_filters["Linear 5x5"] = new FilterLinear5x5();
  m_filters["Median"] = new FilterMedian();
}


void imgproc::onFilterAction ()
{
  QAction *action = (QAction *)sender();
  Filter *filter = m_filters[action->text()];
  if (!filter)
    return;
  ActionApplyFilter *applyAction = new ActionApplyFilter(*filter);
  if (!applyAction->PrepareFilterParams(this)) {
    return;
  }
  
  m_undoManager.pushAction(applyAction);
  QImage img = m_originalPixmap.toImage();
  applyAction->Redo(img);
  m_originalPixmap = QPixmap::fromImage(img);
  ui.imageLabel->setPixmap(m_originalPixmap);
  
}

void imgproc::showAbout()
{
  QMessageBox::about(this, "About", "Image processing by GroupA (Churinon Dmitriy, Paramonov Vlad) \t\n");
}

void imgproc::showSettings()
{
  SettingsDialog dlg;
  dlg.exec();
}

void imgproc::compare()
{
  ImageComparator cmp;
  cmp.Compare(m_currFilePath, ui.imageLabel->pixmap()->toImage());
}
