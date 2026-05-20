#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QString>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

private slots:
  void onConvertClicked();
  void onLoadClicked();
  void onSaveClicked();

private:
  void setError(const QString& text);
  void setResult(const QString& text);

private:
  QLineEdit* le_p = nullptr;
  QLineEdit* le_q = nullptr;
  QLineEdit* le_input = nullptr;

  QTextEdit* te_output = nullptr;
  QTextEdit* te_errors = nullptr;

  QPushButton* btn_convert = nullptr;
  QPushButton* btn_load = nullptr;
  QPushButton* btn_save = nullptr;
};
