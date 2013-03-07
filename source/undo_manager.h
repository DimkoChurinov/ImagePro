#pragma once

#include <QObject>
#include <QVector>

class QImage;
class Action;
class UndoManager : public QObject {
  Q_OBJECT
public:
  void clear();

  bool canRedo() const { return !m_redoActionsStack.empty(); }
  bool canUndo() const { return !m_undoActionsStack.empty(); }

  void redo(QImage &image, int redoItemsCount = 1);
  void undo(QImage &image, int undoItemsCount = 1);

  void pushAction(Action *action);

signals:
  void historyChanged();

private:
  QVector<Action *> m_undoActionsStack;
  QVector<Action *> m_redoActionsStack;
};