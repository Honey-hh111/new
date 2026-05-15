#include "sokoban.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>

Sokoban::Sokoban(QWidget *parent)
    : QWidget(parent)
    , currentLevel(1)
    , gameCompleted(false)
    , playerDirection(1)  // 默认向下
{
    // 窗口设置
    setWindowTitle("📦 推箱子");
    setFixedSize(650, 750);
    setWindowFlags(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);

    // 不透明背景
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(44, 62, 80));
    setPalette(pal);

    setupUI();
    loadImages();
    loadLevel(1);
}

void Sokoban::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel("📦 推箱子 Sokoban", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; color: white; padding: 10px;");
    mainLayout->addWidget(title);

    // 改成 QLabel 网格
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            cells[row][col] = new QLabel(this);
            cells[row][col]->setFixedSize(48, 48);
            cells[row][col]->setScaledContents(true);  // 让图片自动缩放
            cells[row][col]->setAlignment(Qt::AlignCenter);
            cells[row][col]->setStyleSheet("background-color: transparent;");
            gridLayout->addWidget(cells[row][col], row, col);
        }
    }
    mainLayout->addLayout(gridLayout);

    // 控制栏
    QHBoxLayout* controlLayout = new QHBoxLayout();

    QPushButton* resetBtn = new QPushButton("🔄 重置关卡", this);
    QPushButton* nextBtn = new QPushButton("⏩ 下一关", this);
    QPushButton* pauseBtn = new QPushButton("⏸ 暂停", this);
    QPushButton* exitBtn = new QPushButton("❌ 退出", this);
    statusLabel = new QLabel("第 1 / 10 关", this);

    statusLabel->setStyleSheet("color: white; font-size: 14px;");
    resetBtn->setFixedSize(100, 35);
    nextBtn->setFixedSize(100, 35);
    pauseBtn->setFixedSize(100, 35);
    exitBtn->setFixedSize(100, 35);

    connect(resetBtn, &QPushButton::clicked, this, &Sokoban::resetLevel);
    connect(nextBtn, &QPushButton::clicked, this, &Sokoban::nextLevel);
    connect(pauseBtn, &QPushButton::clicked, this, &Sokoban::pauseGame);
    connect(exitBtn, &QPushButton::clicked, this, &Sokoban::exitGame);

    controlLayout->addWidget(resetBtn);
    controlLayout->addWidget(nextBtn);
    controlLayout->addWidget(pauseBtn);
    controlLayout->addWidget(exitBtn);
    controlLayout->addStretch();
    controlLayout->addWidget(statusLabel);
    mainLayout->addLayout(controlLayout);

    QLabel* tip = new QLabel("使用方向键 ↑ ↓ ← → 移动小人", this);
    tip->setAlignment(Qt::AlignCenter);
    tip->setStyleSheet("color: #aaa; padding: 10px;");
    mainLayout->addWidget(tip);

    setLayout(mainLayout);
    setFocusPolicy(Qt::StrongFocus);
}

void Sokoban::loadImages()
{
    // 加载方向图片
    auto loadDirectionImage = [this](QPixmap& img, const QString& path, QColor color, const QString& arrow) {
        if (!img.load(path)) {
            img = QPixmap(48, 48);
            img.fill(color);
            QPainter painter(&img);
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 24));
            painter.drawText(img.rect(), Qt::AlignCenter, arrow);
        }
    };

    loadDirectionImage(playerImgUp, ":/images/player_up.png", Qt::green, "↑");
    loadDirectionImage(playerImgDown, ":/images/player_down.png", Qt::green, "↓");
    loadDirectionImage(playerImgLeft, ":/images/player_left.png", Qt::green, "←");
    loadDirectionImage(playerImgRight, ":/images/player_right.png", Qt::green, "→");

    // 加载其他图片
    auto loadOrFallback = [](QPixmap& img, const QString& path, QColor color) {
        if (!img.load(path)) {
            img = QPixmap(48, 48);
            img.fill(color);
        }
    };

    loadOrFallback(boxImg, ":/images/box.png", QColor(210, 150, 75));
    loadOrFallback(wallImg, ":/images/wall.png", QColor(80, 80, 80));
    loadOrFallback(targetImg, ":/images/target.png", QColor(255, 100, 100));
    loadOrFallback(floorImg, ":/images/floor.png", QColor(50, 50, 60));
    loadOrFallback(boxOnTargetImg, ":/images/box_on_target.png", QColor(150, 200, 150));
}

void Sokoban::loadLevel(int level)
{    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            map[i][j] = 0;
        }
    }

    // 设置外边界墙壁 (固定边界)
    for (int i = 0; i < 12; i++) {
        map[0][i] = 2;      // 上边界
        map[11][i] = 2;     // 下边界
        map[i][0] = 2;      // 左边界
        map[i][11] = 2;     // 右边界
    }

    // 根据关卡数初始化不同的关卡配置
    switch (level) {

    case 1: {
        // 关卡1 - 1个箱子，简单绕行
        playerX = 5; playerY = 5;
        map[playerX][playerY] = 1;
        map[3][5] = 3;      // 箱子
        map[8][5] = 4;      // 目标
        // 障碍物阻挡直线
        map[4][5] = 2;
        map[6][5] = 2;
        map[5][4] = 2;
        break;
    }

    case 2: {
        // 关卡2 - 2个箱子，L形布局
        playerX = 7; playerY = 3;
        map[playerX][playerY] = 1;
        map[3][4] = 3;
        map[8][7] = 3;
        map[4][8] = 4;
        map[9][3] = 4;
        // 阻挡直线
        map[3][5] = 2; map[3][6] = 2;
        map[7][7] = 2; map[7][8] = 2;
        map[5][3] = 2; map[6][3] = 2;
        break;
    }

    case 3: {
        playerX = 4; playerY = 8;
        map[playerX][playerY] = 1;
        map[2][3] = 3;map[7][8] = 3;
        map[2][7] = 4;map[9][4] = 4;
        map[4][4] = 2; map[4][5] = 2; map[5][4] = 2;
        map[5][6] = 2; map[6][5] = 2;
        map[2][4] = 2; map[2][5] = 2;
        break;
    }

    case 4: {
        playerX = 6; playerY = 4;
        map[playerX][playerY] = 1;
        map[2][6] = 3;map[8][3] = 3;
        map[3][9] = 4;map[9][6] = 4;
        for (int i = 3; i <= 8; i++) {
            if (i != 5) map[4][i] = 2;
            if (i != 7) map[7][i] = 2;
        }
        map[5][3] = 2; map[6][8] = 2;
        break;
    }

    case 5: {
        playerX = 5; playerY = 5;
        map[playerX][playerY] = 1;
        map[2][4] = 3;map[3][8] = 3;map[7][3] = 3;
        map[4][2] = 4;map[8][7] = 4;map[9][5] = 4;
        map[3][3] = 2; map[3][4] = 2;
        map[4][7] = 2; map[5][8] = 2;
        map[6][4] = 2; map[7][5] = 2;
        map[8][4] = 2;
        break;
    }

    case 6: {
        playerX = 8; playerY = 8;
        map[playerX][playerY] = 1;
        map[2][2] = 3;map[4][7] = 3;map[7][4] = 3;
        map[3][5] = 4;map[6][8] = 4;map[9][3] = 4;
        for (int i = 3; i <= 7; i++) {
            if (i != 4) map[3][i] = 2;
            if (i != 6) map[6][i] = 2;
        }
        map[4][3] = 2; map[5][5] = 2;
        map[7][7] = 2;
        break;
    }

    case 7: {
        playerX = 4; playerY = 6;
        map[playerX][playerY] = 1;
        map[2][5] = 3;map[5][9] = 3;map[8][2] = 3;
        map[3][3] = 4;map[7][7] = 4;map[9][8] = 4;
        for (int i = 3; i <= 7; i++) {
            if (i != 5) map[4][i] = 2;
            if (i != 4) map[7][i] = 2;
        }
        map[3][6] = 2; map[5][4] = 2;
        map[6][5] = 2; map[8][5] = 2;
        break;
    }

    case 8: {
        playerX = 6; playerY = 3;
        map[playerX][playerY] = 1;
        map[2][6] = 3;map[3][2] = 3;map[6][9] = 3;map[9][4] = 3;
        map[2][3] = 4;map[4][9] = 4;map[8][2] = 4;map[9][7] = 4;
        for (int i = 3; i <= 8; i++) {
            if (i != 4 && i != 6) map[3][i] = 2;
            if (i != 5) map[6][i] = 2;
        }
        map[4][4] = 2; map[5][3] = 2;
        map[7][6] = 2; map[8][5] = 2;
        break;
    }

    case 9: {
        playerX = 5; playerY = 5;
        map[playerX][playerY] = 1;
        map[2][4] = 3;
        map[3][8] = 3;
        map[7][2] = 3;
        map[8][7] = 3;
        map[2][7] = 4;
        map[4][3] = 4;
        map[7][8] = 4;
        map[9][5] = 4;

        for (int i = 3; i <= 8; i++) {
            if (i != 5) map[4][i] = 2;
            if (i != 4) map[7][i] = 2;
        }
        map[3][4] = 2; map[5][6] = 2;
        map[6][5] = 2; map[8][4] = 2;
        break;
    }

    case 10: {
        playerX = 7; playerY = 7;
        map[playerX][playerY] = 1;
        map[2][3] = 3;map[3][7] = 3; map[5][9] = 3; map[7][2] = 3;map[9][6] = 3;
        map[2][8] = 4;map[4][4] = 4; map[6][3] = 4;map[8][9] = 4;map[9][2] = 4;
        for (int i = 3; i <= 8; i++) {
            if (i != 4 && i != 7) map[3][i] = 2;
            if (i != 5) map[6][i] = 2;
            if (i != 6) map[8][i] = 2;
        }
        map[4][5] = 2; map[4][6] = 2;
        map[5][4] = 2; map[5][7] = 2;
        map[7][5] = 2; map[7][8] = 2;
        break;
    }

    default:
        break;
    }

    drawMap();
    statusLabel->setText(QString("第 %1 / 10 关").arg(level));
}

void Sokoban::drawMap()
{
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            QPixmap pixmap;

            switch (map[row][col]) {
            case 0:  // 空地
                pixmap = floorImg;
                break;
            case 1:  // 玩家
                switch (playerDirection) {
                case 0: pixmap = playerImgUp; break;
                case 1: pixmap = playerImgDown; break;
                case 2: pixmap = playerImgLeft; break;
                case 3: pixmap = playerImgRight; break;
                default: pixmap = playerImgDown;
                }
                break;
            case 2:  // 墙
                pixmap = wallImg;
                break;
            case 3:  // 箱子
                pixmap = boxImg;
                break;
            case 4:  // 目标点
                pixmap = targetImg;
                break;
            case 5:  // 箱子在目标点上
                pixmap = boxOnTargetImg;
                break;
            }

            if (!pixmap.isNull()) {
                QPixmap scaled = pixmap.scaled(46, 46, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                cells[row][col]->setPixmap(scaled);
            } else {
                cells[row][col]->setPixmap(QPixmap());  // 清空
            }
        }
    }
}

bool Sokoban::move(int dx, int dy)
{
    int newX = playerX + dx;
    int newY = playerY + dy;

    // 边界检查
    if (newX < 0 || newX >= ROWS || newY < 0 || newY >= COLS) {
        return false;
    }

    // 碰到墙
    if (map[newX][newY] == 2) return false;

    // 更新玩家方向
    if (dx == -1) playerDirection = 0;
    else if (dx == 1) playerDirection = 1;
    else if (dy == -1) playerDirection = 2;
    else if (dy == 1) playerDirection = 3;

    // 记录原玩家位置是否为目标点
    bool wasOnTarget = (map[playerX][playerY] == 4);

    // 推到箱子
    if (map[newX][newY] == 3 || map[newX][newY] == 5) {
        int pushX = newX + dx;
        int pushY = newY + dy;

        if (pushX < 0 || pushX >= ROWS || pushY < 0 || pushY >= COLS) {
            return false;
        }

        if (map[pushX][pushY] == 0 || map[pushX][pushY] == 4) {
            // 移动箱子
            if (map[pushX][pushY] == 4) {
                map[pushX][pushY] = 5;
            } else {
                map[pushX][pushY] = 3;
            }

            // 原箱子位置变成玩家
            map[newX][newY] = 1;

            // 原玩家位置处理
            if (wasOnTarget) {
                map[playerX][playerY] = 4;  // 恢复目标点
            } else {
                map[playerX][playerY] = 0;  // 空地
            }

            playerX = newX;
            playerY = newY;

            drawMap();

            // 胜利检测
            bool win = true;
            for (int i = 0; i < ROWS && win; i++) {
                for (int j = 0; j < COLS; j++) {
                    if (map[i][j] == 4) {
                        win = false;
                        break;
                    }
                }
            }

            if (win && !gameCompleted) {
                gameCompleted = true;
                if (currentLevel < 10) {
                    QMessageBox::information(this, "🎉 过关！",
                                             QString("恭喜通过第 %1 关！\n进入下一关？").arg(currentLevel));
                    nextLevel();
                } else {
                    QMessageBox::information(this, "🎉 通关！", "恭喜完成所有关卡！");
                }
            }
            return true;
        }
        return false;
    }

    if (map[newX][newY] == 4) {
        map[newX][newY] = 1;  // 玩家站到目标点上
    } else {
        map[newX][newY] = 1;  // 玩家站到空地上
    }

    // 清空原玩家位置（关键！）
    if (wasOnTarget) {
        map[playerX][playerY] = 4;  // 恢复目标点
    } else {
        map[playerX][playerY] = 0;  // 空地
    }

    playerX = newX;
    playerY = newY;

    drawMap();
    return true;
}
void Sokoban::keyPressEvent(QKeyEvent* event)
{
    if (gameCompleted) return;

    switch (event->key()) {
    case Qt::Key_Up:
        playerDirection = 0;
        move(-1, 0);
        break;
    case Qt::Key_Down:
        playerDirection = 1;
        move(1, 0);
        break;
    case Qt::Key_Left:
        playerDirection = 2;
        move(0, -1);
        break;
    case Qt::Key_Right:
        playerDirection = 3;
        move(0, 1);
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void Sokoban::resetLevel()
{
    loadLevel(currentLevel);
    gameCompleted = false;
}

void Sokoban::nextLevel()
{
    if (currentLevel < 10) {
        currentLevel++;
        loadLevel(currentLevel);
        gameCompleted = false;
    } else {
        QMessageBox::information(this, "提示", "恭喜你完成了所有关卡！");
    }
}

void Sokoban::pauseGame()
{
    QMessageBox::information(this, "⏸ 暂停", "推箱子已暂停\n点击确定继续");
}

void Sokoban::exitGame()
{
    this->close();
}