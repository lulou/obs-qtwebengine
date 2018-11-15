
bool QWebEngineViewCustom::eventFilter(QObject* object, QEvent* event) {
 if (event->type() == QEvent::UpdateRequest) {
  emit updateFinished();
 }
}