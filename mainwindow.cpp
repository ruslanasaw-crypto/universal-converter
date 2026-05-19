// mainwindow.cpp
#include "mainwindow.h"
#include "bigfrac.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
QString readAndPrintFraction(const QString& input_str, bool& success, QString& error_msg) {
  std::istringstream iss(input_str.toStdString());
  BigFraction f;
  if (!(iss >> f)) {
    success = false;
    error_msg = "Не удалось разобрать дробь...";
    return QString();
  }

  std::ostringstream oss;
  oss << f;
  success = true;
  error_msg.clear();
  return QString::fromStdString(oss.str());
}
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
  QWidget* central = new QWidget(this);
  setCentralWidget(central);

  le_input = new QLineEdit();

  btn_convert = new QPushButton("Show fraction");
  te_output = new QTextEdit;
  te_output->setReadOnly(true);

  te_errors = new QTextEdit;
  te_errors->setReadOnly(true);

  btn_load = new QPushButton("Load from file...");
  btn_save = new QPushButton("Save to file...");

         // layout
  QVBoxLayout* v1 = new QVBoxLayout;
  v1->addWidget(new QLabel("Input fraction"));
  v1->addWidget(le_input);
  v1->addWidget(btn_convert);

  QVBoxLayout* v2 = new QVBoxLayout;
  v2->addWidget(new QLabel("Result (BigFraction as string):"));
  v2->addWidget(te_output);
  v2->addWidget(new QLabel("Errors / Notes:"));
  v2->addWidget(te_errors);

  QHBoxLayout* h1 = new QHBoxLayout;
  h1->addWidget(btn_load);
  h1->addWidget(btn_save);
  h1->addStretch();

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addLayout(v1);
  mainLayout->addLayout(v2);
  mainLayout->addLayout(h1);

  central->setLayout(mainLayout);

  connect(btn_convert, &QPushButton::clicked,
          this, &MainWindow::onConvertClicked);
  connect(btn_load, &QPushButton::clicked,
          this, &MainWindow::onLoadClicked);
  connect(btn_save, &QPushButton::clicked,
          this, &MainWindow::onSaveClicked);
}

void MainWindow::onConvertClicked() {
  QString input = le_input->text().trimmed();
  std::cout << "Input string: \"" << input.toStdString() << "\"" << std::endl;

  std::istringstream iss(input.toStdString());
  BigFraction f;
  if (!(iss >> f)) {
    showResult("", "Ошибка: строка ввода пуста.");
    return;
  }


  bool success = true;
  QString error_msg;
  QString result = readAndPrintFraction(input, success, error_msg);

  if (success) {
    showResult(result, error_msg);
  } else {
    showResult("", error_msg);
  }
}

void MainWindow::showResult(const QString& result, const QString& warning) {
  te_output->clear();
  if (!result.isEmpty()) {
    te_output->append(result);
  }
  te_errors->clear();
  if (!warning.isEmpty()) {
    te_errors->append(warning);
  }
}

void MainWindow::onLoadClicked() {
  QString path = QFileDialog::getOpenFileName(this, "Загрузить дробь из файла...",
                                              QString(), "Text files (*.txt)");
  if (path.isEmpty()) return;

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл: " + file.errorString());
    return;
  }

  QTextStream ts(&file);
  QString content = ts.readAll();
  file.close();

  le_input->setText(content.trimmed());
}

void MainWindow::onSaveClicked() {
  QString text = te_output->toPlainText();
  if (text.isEmpty()) {
    QMessageBox::information(this, "Сохранение", "Нет данных для сохранения.");
    return;
  }

  QString path = QFileDialog::getSaveFileName(this, "Сохранить дробь в файл...",
                                              QString(), "Text files (*.txt)");
  if (path.isEmpty()) return;

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
    return;
  }

  QTextStream ts(&file);
  ts << text;
  file.close();
}
