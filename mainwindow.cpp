#include "mainwindow.h"
#include "sudoku.h"
#include "solitaire.h"
#include "sokoban.h"
#include <QMessageBox>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,musicPlayer(nullptr)
    ,audioOutput(nullptr)
{
    setupUI();
    initMusic();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("🎮 游戏合集");
    setFixedSize(400, 500);

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    // 标题
    QLabel* title = new QLabel("🎮 选择游戏 🎮", central);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #4F48CF; padding: 30px;");
    layout->addWidget(title);

    // 三个游戏按钮
    QPushButton* sudokuBtn = new QPushButton("🔢 数独 Sudoku", central);
    QPushButton* solitaireBtn = new QPushButton("🃏 纸牌接龙 Solitaire", central);
    QPushButton* sokobanBtn = new QPushButton("📦 推箱子 Sokoban", central);

    sudokuBtn->setFixedHeight(60);
    solitaireBtn->setFixedHeight(60);
    sokobanBtn->setFixedHeight(60);

    sudokuBtn->setStyleSheet("font-size: 18px; background-color: #4a90d9; color: white; border-radius: 10px;");
    solitaireBtn->setStyleSheet("font-size: 18px; background-color: #e67e22; color: white; border-radius: 10px;");
    sokobanBtn->setStyleSheet("font-size: 18px; background-color: #27ae60; color: white; border-radius: 10px;");

    connect(sudokuBtn, &QPushButton::clicked, this, &MainWindow::openSudoku);
    connect(solitaireBtn, &QPushButton::clicked, this, &MainWindow::openSolitaire);
    connect(sokobanBtn, &QPushButton::clicked, this, &MainWindow::openSokoban);

    layout->addWidget(sudokuBtn);
    layout->addWidget(solitaireBtn);
    layout->addWidget(sokobanBtn);
    layout->addStretch();

    setCentralWidget(central);

    // 右下角音量按钮
    QPushButton* volumeBtn = new QPushButton("🔊", central);
    volumeBtn->setFixedSize(45, 45);
    volumeBtn->setStyleSheet(
        "border-radius: 22px;"
        "background-color: #3a3a4a;"
        "color: white;"
        "font-size: 20px;"
        "border: 2px solid #5a5a6a;"
        );
    volumeBtn->move(width() - 60, height() - 70);

    // 音量状态
    static int volumeLevel = 50;  // 0-100
    connect(volumeBtn, &QPushButton::clicked, [this, volumeBtn]() {
        volumeLevel += 25;
        if (volumeLevel > 100) volumeLevel = 0;

        float vol = volumeLevel / 100.0f;
        if (audioOutput) audioOutput->setVolume(vol);

        // 更新图标
        if (volumeLevel == 0) volumeBtn->setText("🔇");
        else if (volumeLevel <= 25) volumeBtn->setText("🔉");
        else if (volumeLevel <= 50) volumeBtn->setText("🔊");
        else if (volumeLevel <= 75) volumeBtn->setText("🍎");
        else volumeBtn->setText("🥰");
    });
}

void MainWindow::openSudoku()
{
    Sudoku* sudoku = new Sudoku(this);
    sudoku->show();
}

void MainWindow::openSolitaire()
{
    Solitaire* solitaire = new Solitaire(this);
    solitaire->show();
}

void MainWindow::openSokoban()
{
    Sokoban* sokoban = new Sokoban(this);
    sokoban->show();
}

void MainWindow::initMusic()
{
    // 创建音频输出
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.5);

    // 创建播放器
    musicPlayer = new QMediaPlayer(this);
    musicPlayer->setAudioOutput(audioOutput);

    // 加载音乐（文件放在 build/debug/ 目录下）
    musicPlayer->setSource(QUrl::fromLocalFile("bgm.mp3"));
    musicPlayer->play();

    // 循环播放
    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged,
            [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia) {
                    musicPlayer->setPosition(0);
                    musicPlayer->play();
                }
            });
}