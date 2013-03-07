
#include <QImage>
#include "undo_manager.h"
#include "action.h"

void UndoManager::pushAction (Action *action)
{
  m_undoActionsStack.push_back(action);
  for (int i = 0; i < m_redoActionsStack.size(); ++i) {
    delete m_redoActionsStack[i];
  }
  m_redoActionsStack.clear();
  emit historyChanged();
}

void UndoManager::redo (QImage &image, int redoItemsCount /*= 1*/)
{
  if (canRedo()) {
    // TODO: handle redoItemsCount
    m_redoActionsStack.back()->Redo(image);
    m_undoActionsStack.push_back(m_redoActionsStack.back());
    m_redoActionsStack.pop_back();
    emit historyChanged();
  }
}

void UndoManager::undo (QImage &image, int undoItemsCount /*= 1*/)
{
  if (canUndo()) {
    // TODO: handle undoItemsCount
    m_undoActionsStack.back()->Undo(image);
    m_redoActionsStack.push_back(m_undoActionsStack.back());
    m_undoActionsStack.pop_back();
    emit historyChanged();
  }
}

void UndoManager::clear ()
{
  for (int i = 0; i < m_redoActionsStack.size(); ++i) {
    delete m_redoActionsStack[i];
  }
  for (int i = 0; i < m_undoActionsStack.size(); ++i) {
    delete m_undoActionsStack[i];
  }
  m_redoActionsStack.clear();
  m_undoActionsStack.clear();
  emit historyChanged();
}