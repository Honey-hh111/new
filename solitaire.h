#ifndef SOLITAIRE_H
#define SOLITAIRE_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QPixmap>
#include <QLabel>

class Solitaire : public QWidget
{
    Q_OBJECT

public:
    explicit Solitaire(QWidget *parent = nullptr);

private slots:
    void dealNewGame();
    void pauseGame();
    void exitGame();

private:
    void setupUI();
    void loadCardImages();
    void dealCards();
    void onWasteClicked();
    void onCardClicked(int col,int cow);

    // 游戏数据结构
    struct Card {
        int suit;   // ♠️,♥️,♣️,♦️
        int rank;   // 1-13 (1=A,11=J,12=Q,13=K)
        bool faceUp;
        Card(int s=0, int r=0, bool f=false) : suit(s), rank(r), faceUp(f) {}
    };

    QVector<Card> stock;      // 牌堆
    QVector<Card> waste;      // 废牌堆
    QVector<Card> tableau[7]; // 7列牌堆
    QVector<Card> foundation[4]; // 4个基础牌堆

    QVector<QPushButton*> tableauBtns[7];
    QVector<QPushButton*> foundationBtns;
    QPushButton* stockBtn;
    QPushButton* wasteBtn;

    QPixmap cardBack;
    QPixmap cardFronts[52];

    QLabel* statusLabel;
    bool gameStarted;

    static const int CARD_WIDTH = 64;
    static const int CARD_HEIGHT = 80;

    // 移动逻辑函数
    bool canMoveToTableau(const Card& card, int col);      // 能否移动到牌列
    bool canMoveToFoundation(const Card& card, int suit);  // 能否移动到基础牌堆
    void moveToFoundation(int col, int cardIndex);         // 从牌列移动到基础牌堆
    void moveToTableau(int fromCol, int fromIndex, int toCol);  // 牌列间移动
    void autoMoveToFoundation();                           // 自动移动可放的牌
    void dealFromStock();                                  // 从牌堆发牌
    void updateDisplay();                                  // 刷新显示（已有）

    // 辅助函数
    bool isRedSuit(const Card& card);                      // 是否为红色花色
    bool isBlackSuit(const Card& card);                    // 是否为黑色花色
    QString cardToText(const Card& card);                  // 牌转文字（调试用）

};

#endif