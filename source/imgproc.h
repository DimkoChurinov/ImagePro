#ifndef IMGPROC_H
#define IMGPROC_H

#include <QtGui/QMainWindow>
#include <QScrollArea>
#include "ui_imgproc.h"

#include "undo_manager.h"
#include "action.h"
#include "filter.h"

class imgproc : public QMainWindow
{
    Q_OBJECT

public:
    imgproc(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~imgproc();
private:
  void updateActions();
  void scaleImage(double factor);
  void adjustScrollBar(QScrollBar *scrollBar, double factor);
  void wheelEvent(QWheelEvent *event);

  void createActionsUI( Ui::imgprocClass &ui );
  void loadFilters();

private slots:
  void open();
  void save();
  void zoomIn();
  void zoomOut();
  void normalSize();
  void fitToWindow();
  void showAbout();
  void redo();
  void undo();
  void showSettings();
  void compare();

  void onHistoryChanged();
  void onFilterAction();

private:
    Ui::imgprocClass ui;    
    QPixmap m_originalPixmap;
    QScrollArea *m_scrollArea;
    double m_scaleFactor;

    UndoManager m_undoManager;

    QMap<QString, Filter *> m_filters;
    QVector<QAction *> m_filterActions;
    QString m_currFilePath;
};

#endif // IMGPROC_H
