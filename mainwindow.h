#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QAudioOutput>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QMediaPlayer* musicPlayer;
    QAudioOutput* audioOutput;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSudoku();
    void openSolitaire();
    void openSokoban();

private:
    void setupUI();
    void initMusic();
};

#endif
