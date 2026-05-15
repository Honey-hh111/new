#ifndef SOKOBAN_H
#define SOKOBAN_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include <QPixmap>

class Sokoban : public QWidget
{
    Q_OBJECT

public:
    explicit Sokoban(QWidget *parent = nullptr);

private slots:
    void resetLevel();
    void nextLevel();
    void pauseGame();
    void exitGame();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupUI();
    void loadImages();
    void loadLevel(int level);
    void drawMap();
    bool move(int dx, int dy);

    static const int ROWS = 12;
    static const int COLS = 12;

    int map[ROWS][COLS];
    int playerX, playerY;
    int currentLevel;
    bool gameCompleted;

    // 方向图片
    QPixmap playerImgUp;
    QPixmap playerImgDown;
    QPixmap playerImgLeft;
    QPixmap playerImgRight;
    int playerDirection;  // 0=上,1=下,2=左,3=右

    QPixmap boxImg;
    QPixmap wallImg;
    QPixmap targetImg;
    QPixmap floorImg;
    QPixmap boxOnTargetImg;

    QLabel* cells[ROWS][COLS];
    QLabel* statusLabel;
};

#endif