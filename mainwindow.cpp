#include "mainwindow.h"
#include "parser.h"
#include "formatter.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
  QWidget* central = new QWidget(this);
  setCentralWidget(central);

  le_p = new QLineEdit();
  le_q = new QLineEdit();
  le_input = new QLineEdit();

  btn_convert = new QPushButton("Convert");

  te_output = new QTextEdit;
  te_output->setReadOnly(true);
  te_errors = new QTextEdit;
  te_errors->setReadOnly(true);

  btn_load = new QPushButton("Load from file...");
  btn_save = new QPushButton("Save to file...");

         // layout
  QGridLayout* gl = new QGridLayout;
  gl->addWidget(new QLabel("p [2, 500]:"), 0, 0);
  gl->addWidget(le_p, 0, 1);
  gl->addWidget(new QLabel("q [2, 500]:"), 1, 0);
  gl->addWidget(le_q, 1, 1);
  gl->addWidget(new QLabel("input:"), 2, 0);
  gl->addWidget(le_input, 2, 1, 1, 2);

  gl->addWidget(btn_convert, 3, 0, 1, 3);

  QVBoxLayout* v1 = new QVBoxLayout;
  v1->addWidget(new QLabel("output:"));
  v1->addWidget(te_output);
  v1->addWidget(new QLabel("Errors"));
  v1->addWidget(te_errors);

  QHBoxLayout* h1 = new QHBoxLayout;
  h1->addWidget(btn_load);
  h1->addWidget(btn_save);
  h1->addStretch();

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gl);
  mainLayout->addLayout(v1);
  mainLayout->addLayout(h1);

  central->setLayout(mainLayout);

  connect(btn_convert, &QPushButton::clicked, this, &MainWindow::onConvertClicked);
  connect(btn_load, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
  connect(btn_save, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
}

void MainWindow::showResult(const QString& result, const QString& warning) {
  te_output->clear();
  if (!result.isEmpty()) te_output->setPlainText(result);
  te_errors->clear();
  if (!warning.isEmpty()) {
    te_errors->setPlainText(warning);
  }
}

void MainWindow::onConvertClicked() {
  bool ok;
  QString s_p = le_p->text().trimmed();
  QString s_q = le_q->text().trimmed();
  QString input = le_input->text().trimmed();

  int p = s_p.toInt(&ok);
  if (!ok || p < 2 || p > 500) {
    showResult("", "Ошибка: p должно быть целым числом от 2 до 500.");
    return;
  }

  int q = s_q.toInt(&ok);
  if (!ok || q < 2 || q > 500) {
    showResult("", "Ошибка: q должно быть целым числом от 2 до 500.");
    return;
  }

  if (input.isEmpty()) {
    showResult("", "Ошибка: входная строка пуста.");
    return;
  }

  try {
    BigFraction x = parse_p_number(input.toStdString(), p);
    BigInteger I = x.integerPart();
    std::string int_str = integerToBaseQ(I, q);
    showResult(QString::fromStdString(int_str), QString());
  } catch (const std::exception& e) {
    showResult("", QString::fromStdString(e.what()));
  }
}

void MainWindow::onLoadClicked() {
  QString path = QFileDialog::getOpenFileName(this, "Загрузить файл...", "", "Text files (*.txt)");
  if (path.isEmpty()) return;

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл: " + file.errorString());
    return;
  }

  QTextStream ts(&file);
  QString p_str = ts.readLine().trimmed();
  QString q_str = ts.readLine().trimmed();
  QString inp = ts.readAll().trimmed();
  file.close();

  le_p->setText(p_str);
  le_q->setText(q_str);
  le_input->setText(inp);
}

void MainWindow::onSaveClicked() {
  QString text = te_output->toPlainText();
  if (text.isEmpty()) {
    QMessageBox::information(this, "Сохранение", "Нет данных для сохранения.");
    return;
  }

  QString path = QFileDialog::getSaveFileName(this, "Сохранить результат...", "", "Text files (*.txt)");
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
