#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

class BigInteger;
class BigFraction;
BigFraction parse_p_number(const std::string& input, int p);
std::string integerToBaseQ(const BigInteger& integer, int q);

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);

private slots:
  void onConvertClicked();
  void onLoadClicked();
  void onSaveClicked();

private:
  void showResult(const QString& result, const QString& warning = QString());

  QLineEdit* le_p;
  QLineEdit* le_q;
  QLineEdit* le_input;

  QTextEdit* te_output;
  QTextEdit* te_errors;

  QPushButton* btn_convert;
  QPushButton* btn_load;
  QPushButton* btn_save;
};
