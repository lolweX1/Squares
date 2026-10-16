#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <vector>
#include "Rectangle.hpp"

class Window : public QGraphicsView {
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

    Rectangle* createRectangle(float x, float y, float width, float height, 
                               float mass, float angle, 
                               Vec2 init_lin_velo = {0.0f, 0.0f}, 
                               float init_ang_velo = 0.0f,
                               QColor color = Qt::cyan);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateGameLoop();
    void applySpinBoxValues1(); // Syncs Panel 1 to Square 1
    void applySpinBoxValues2(); // Syncs Panel 2 to Square 2

private:
    QGraphicsScene *scene;
    QTimer *timer;
    std::vector<Rectangle*> rectangles;

    float totalTime = 0.0f;
    bool isPaused = false;

    QGraphicsTextItem *hudText;

    // Panel 1 Controls (Cyan Square)
    QWidget *panel1Widget;
    QDoubleSpinBox *mass1SpinBox;
    QDoubleSpinBox *velX1SpinBox;
    QDoubleSpinBox *velY1SpinBox;
    QDoubleSpinBox *angVel1SpinBox;

    // Panel 2 Controls (Orange Square)
    QWidget *panel2Widget;
    QDoubleSpinBox *mass2SpinBox;
    QDoubleSpinBox *velX2SpinBox;
    QDoubleSpinBox *velY2SpinBox;
    QDoubleSpinBox *angVel2SpinBox;

    void setupControlPanels();
    void updateHUD();
    void updatePhysics(float dt);
    void setPanelsVisible(bool visible);
};

#endif // WINDOW_HPP