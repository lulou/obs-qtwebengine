class QWebEngineViewCustom : public QWebEngineView {
Q_OBJECT
 public:
  QWebEngineViewCustom(QWidget *parent = nullptr);
 protected:
  void contextMenuEvent(QContextMenuEvent *event) override;
  QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;
 signals:
  void webActionEnabledChanged(QWebEnginePage::WebAction webAction, bool enabled);
};