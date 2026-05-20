#include "mainwindow.h"
#include "parser.h"
#include "formatter.h"
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
  auto* central = new QWidget(this);
  setCentralWidget(central);

  auto* lbl_p = new QLabel("p: [2, 500]", this);
  auto* lbl_q = new QLabel("q: [2, 500]", this);
  auto* lbl_input = new QLabel("input:", this);
  auto* lbl_output = new QLabel("output:", this);
  auto* lbl_errors = new QLabel("errors:", this);

  le_p = new QLineEdit(this);
  le_q = new QLineEdit(this);
  le_input = new QLineEdit(this);

  btn_convert = new QPushButton("Convert", this);
  btn_load = new QPushButton("Load", this);
  btn_save = new QPushButton("Save", this);

  te_output = new QTextEdit(this);
  te_errors = new QTextEdit(this);

  te_output->setReadOnly(true);
  te_errors->setReadOnly(true);

  auto* grid = new QGridLayout;
  grid->addWidget(lbl_p, 0, 0);
  grid->addWidget(le_p, 0, 1);
  grid->addWidget(lbl_q, 1, 0);
  grid->addWidget(le_q, 1, 1);
  grid->addWidget(lbl_input, 2, 0);
  grid->addWidget(le_input, 2, 1, 1, 2);
  grid->addWidget(btn_convert, 3, 0, 1, 3);

  auto* outLayout = new QVBoxLayout;
  outLayout->addWidget(lbl_output);
  outLayout->addWidget(te_output);
  outLayout->addWidget(lbl_errors);
  outLayout->addWidget(te_errors);

  auto* btnLayout = new QHBoxLayout;
  btnLayout->addWidget(btn_load);
  btnLayout->addWidget(btn_save);
  btnLayout->addStretch();

  auto* mainLayout = new QVBoxLayout;
  mainLayout->addLayout(grid);
  mainLayout->addLayout(outLayout);
  mainLayout->addLayout(btnLayout);

  central->setLayout(mainLayout);

  connect(btn_convert, &QPushButton::clicked, this, &MainWindow::onConvertClicked);
  connect(btn_load, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
  connect(btn_save, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
}

void MainWindow::setError(const QString& text) {
  te_errors->setPlainText(text);
  te_output->clear();
}

void MainWindow::setResult(const QString& text) {
  te_output->setPlainText(text);
  te_errors->clear();
}

void MainWindow::onConvertClicked() {
  bool okP = false;
  bool okQ = false;

  int p = le_p->text().trimmed().toInt(&okP);
  int q = le_q->text().trimmed().toInt(&okQ);

  if (!okP || !okQ) {
    setError("Ошибка: основание исходной системы должно быть целым числом от 2 до 500.");
    return;
  }

  if (p < 2 || p > 500 || q < 2 || q > 500) {
    setError("Ошибка: основание должно быть целым числом от 2 до 500.");
    return;
  }

  QString input = le_input->text().trimmed();
  if (input.isEmpty()) {
    setError("Ошибка: входная строка пуста.");
    return;
  }

  try {
    BigFraction value = parseBaseNumber(input.toStdString(), p);
    std::string result = numberToBaseString(value, q);
    setResult(QString::fromStdString(result));
  } catch (const std::exception& e) {
    setError(QString("Ошибка: ") + e.what());
  }
}

void MainWindow::onLoadClicked() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Open input file", QString(), "Text files (*.txt);;All files (*.*)"
      );

  if (fileName.isEmpty()) return;

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    setError("Ошибка: не удалось открыть файл.");
    return;
  }

  QTextStream in(&file);

  QString pLine = in.readLine().trimmed();
  QString qLine = in.readLine().trimmed();
  QString rest = in.readAll().trimmed();

  le_p->setText(pLine);
  le_q->setText(qLine);
  le_input->setText(rest);

  file.close();
}

void MainWindow::onSaveClicked() {
  QString text = te_output->toPlainText().trimmed();
  if (text.isEmpty()) {
    setError("Ошибка: нечего сохранять.");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(
      this, "Save output file", QString(), "Text files (*.txt);;All files (*.*)"
      );

  if (fileName.isEmpty()) return;

  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    setError("Ошибка: не удалось сохранить файл.");
    return;
  }

  QTextStream out(&file);
  out << text;
  file.close();
}
