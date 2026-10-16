#include "Window.hpp"
#include "Collision.hpp"
#include <QBrush>
#include <QColor>
#include <QPen>
#include <QFont>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>

Window::Window(QWidget *parent) : QGraphicsView(parent) {
    setWindowTitle("Squares");

    scene = new QGraphicsScene(this);
    scene->setSceneRect(-400, -300, 800, 600);
    setScene(scene);

    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    setFixedSize(800, 600);

    hudText = new QGraphicsTextItem();
    hudText->setDefaultTextColor(Qt::white);
    hudText->setFont(QFont("Monospace", 10));
    hudText->setPos(-390.0f, -290.0f);
    scene->addItem(hudText);

    setupControlPanels();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Window::updateGameLoop);
    timer->start(16);

    // Spawn Cyan Square (Left) and Orange Square (Right)
    createRectangle(-150.0f, 0.0f, 80.0f, 80.0f, 5.0f, 0.0f, {150.0f, 20.0f}, 45.0f, Qt::cyan);
    createRectangle(150.0f, 0.0f, 80.0f, 80.0f, 3.0f, 15.0f, {-120.0f, -10.0f}, -30.0f, QColor(255, 140, 0));

    updateHUD();
}

Window::~Window() {
    for (Rectangle* rect : rectangles) {
        delete rect;
    }
}

void Window::setupControlPanels() {
    // --- Panel 1 Setup (Cyan Square) ---
    panel1Widget = new QWidget(this);
    panel1Widget->setStyleSheet("background-color: rgba(20, 50, 60, 230); color: #00ffff; border: 1px solid #00ffff; border-radius: 5px;");
    panel1Widget->setGeometry(20, 20, 200, 200);

    QVBoxLayout *layout1 = new QVBoxLayout(panel1Widget);
    QLabel *header1 = new QLabel("<b>Edit Square 1 (Cyan)</b>");
    header1->setAlignment(Qt::AlignCenter);
    layout1->addWidget(header1);

    QHBoxLayout *m1 = new QHBoxLayout(); m1->addWidget(new QLabel("Mass:")); mass1SpinBox = new QDoubleSpinBox(); mass1SpinBox->setRange(0.1, 100); mass1SpinBox->setSingleStep(1); m1->addWidget(mass1SpinBox);
    QHBoxLayout *vx1 = new QHBoxLayout(); vx1->addWidget(new QLabel("Vel X:")); velX1SpinBox = new QDoubleSpinBox(); velX1SpinBox->setRange(-500, 500); velX1SpinBox->setSingleStep(1); vx1->addWidget(velX1SpinBox);
    QHBoxLayout *vy1 = new QHBoxLayout(); vy1->addWidget(new QLabel("Vel Y:")); velY1SpinBox = new QDoubleSpinBox(); velY1SpinBox->setRange(-500, 500); velY1SpinBox->setSingleStep(1); vy1->addWidget(velY1SpinBox);
    QHBoxLayout *w1 = new QHBoxLayout(); w1->addWidget(new QLabel("Ang Vel:")); angVel1SpinBox = new QDoubleSpinBox(); angVel1SpinBox->setRange(-1080, 1080); angVel1SpinBox->setSingleStep(1); w1->addWidget(angVel1SpinBox);

    layout1->addLayout(m1); layout1->addLayout(vx1); layout1->addLayout(vy1); layout1->addLayout(w1);

    connect(mass1SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues1);
    connect(velX1SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues1);
    connect(velY1SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues1);
    connect(angVel1SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues1);

    // --- Panel 2 Setup (Orange Square) ---
    panel2Widget = new QWidget(this);
    panel2Widget->setStyleSheet("background-color: rgba(60, 40, 20, 230); color: #ff9900; border: 1px solid #ff9900; border-radius: 5px;");
    panel2Widget->setGeometry(580, 20, 200, 200);

    QVBoxLayout *layout2 = new QVBoxLayout(panel2Widget);
    QLabel *header2 = new QLabel("<b>Edit Square 2 (Orange)</b>");
    header2->setAlignment(Qt::AlignCenter);
    layout2->addWidget(header2);

    QHBoxLayout *m2 = new QHBoxLayout(); m2->addWidget(new QLabel("Mass:")); mass2SpinBox = new QDoubleSpinBox(); mass2SpinBox->setRange(0.1, 100); mass2SpinBox->setSingleStep(1); m2->addWidget(mass2SpinBox);
    QHBoxLayout *vx2 = new QHBoxLayout(); vx2->addWidget(new QLabel("Vel X:")); velX2SpinBox = new QDoubleSpinBox(); velX2SpinBox->setRange(-500, 500); velX2SpinBox->setSingleStep(1); vx2->addWidget(velX2SpinBox);
    QHBoxLayout *vy2 = new QHBoxLayout(); vy2->addWidget(new QLabel("Vel Y:")); velY2SpinBox = new QDoubleSpinBox(); velY2SpinBox->setRange(-500, 500); velY2SpinBox->setSingleStep(1); vy2->addWidget(velY2SpinBox);
    QHBoxLayout *w2 = new QHBoxLayout(); w2->addWidget(new QLabel("Ang Vel:")); angVel2SpinBox = new QDoubleSpinBox(); angVel2SpinBox->setRange(-1080, 1080); angVel2SpinBox->setSingleStep(1); w2->addWidget(angVel2SpinBox);

    layout2->addLayout(m2); layout2->addLayout(vx2); layout2->addLayout(vy2); layout2->addLayout(w2);

    connect(mass2SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues2);
    connect(velX2SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues2);
    connect(velY2SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues2);
    connect(angVel2SpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Window::applySpinBoxValues2);

    setPanelsVisible(false);
}

void Window::setPanelsVisible(bool visible) {
    if (visible && rectangles.size() >= 2) {
        // Sync Panel 1 to Square 1
        Rectangle* r1 = rectangles[0];
        mass1SpinBox->blockSignals(true); velX1SpinBox->blockSignals(true); velY1SpinBox->blockSignals(true); angVel1SpinBox->blockSignals(true);
        mass1SpinBox->setValue(r1->mass); velX1SpinBox->setValue(r1->linearVelocity.x); velY1SpinBox->setValue(r1->linearVelocity.y); angVel1SpinBox->setValue(r1->angularVelocity);
        mass1SpinBox->blockSignals(false); velX1SpinBox->blockSignals(false); velY1SpinBox->blockSignals(false); angVel1SpinBox->blockSignals(false);

        // Sync Panel 2 to Square 2
        Rectangle* r2 = rectangles[1];
        mass2SpinBox->blockSignals(true); velX2SpinBox->blockSignals(true); velY2SpinBox->blockSignals(true); angVel2SpinBox->blockSignals(true);
        mass2SpinBox->setValue(r2->mass); velX2SpinBox->setValue(r2->linearVelocity.x); velY2SpinBox->setValue(r2->linearVelocity.y); angVel2SpinBox->setValue(r2->angularVelocity);
        mass2SpinBox->blockSignals(false); velX2SpinBox->blockSignals(false); velY2SpinBox->blockSignals(false); angVel2SpinBox->blockSignals(false);
    }
    panel1Widget->setVisible(visible);
    panel2Widget->setVisible(visible);
}

void Window::applySpinBoxValues1() {
    if (isPaused && rectangles.size() >= 1) {
        rectangles[0]->mass = mass1SpinBox->value();
        rectangles[0]->linearVelocity.x = velX1SpinBox->value();
        rectangles[0]->linearVelocity.y = velY1SpinBox->value();
        rectangles[0]->angularVelocity = angVel1SpinBox->value();
    }
}

void Window::applySpinBoxValues2() {
    if (isPaused && rectangles.size() >= 2) {
        rectangles[1]->mass = mass2SpinBox->value();
        rectangles[1]->linearVelocity.x = velX2SpinBox->value();
        rectangles[1]->linearVelocity.y = velY2SpinBox->value();
        rectangles[1]->angularVelocity = angVel2SpinBox->value();
    }
}

Rectangle* Window::createRectangle(float x, float y, float width, float height, 
                               float mass, float angle, 
                               Vec2 init_lin_velo, 
                               float init_ang_velo,
                               QColor color) 
{
    Rectangle* rect = new Rectangle();
    rect->position = {x, y};
    rect->width = width;
    rect->height = height;
    rect->mass = mass;
    rect->angle = angle;
    rect->linearVelocity = init_lin_velo;
    rect->angularVelocity = init_ang_velo;

    QGraphicsRectItem* item = new QGraphicsRectItem(-width / 2.0f, -height / 2.0f, width, height);
    item->setPen(QPen(Qt::white, 2));
    item->setBrush(QBrush(color));

    rect->graphicsItem = item;
    rect->setTransform(x, y, angle);

    scene->addItem(item);
    rectangles.push_back(rect);

    return rect;
}

void Window::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        isPaused = !isPaused;
        setPanelsVisible(isPaused);
    }
    QGraphicsView::keyPressEvent(event);
}

void Window::updatePhysics(float dt) {
    for (Rectangle* rect : rectangles) {
        float newX = rect->position.x + rect->linearVelocity.x * dt;
        float newY = rect->position.y + rect->linearVelocity.y * dt;
        float newAngle = rect->angle + rect->angularVelocity * dt;

        rect->setTransform(newX, newY, newAngle);
    }

    for (Rectangle* rect : rectangles) {
        std::vector<Vec2> corners = getRectangleCorners(rect);
        for (const Vec2& corner : corners) {
            if (corner.x < -400.0f && rect->linearVelocity.x < 0) rect->linearVelocity.x *= -1.0f;
            if (corner.x > 400.0f && rect->linearVelocity.x > 0)  rect->linearVelocity.x *= -1.0f;
            if (corner.y < -300.0f && rect->linearVelocity.y < 0) rect->linearVelocity.y *= -1.0f;
            if (corner.y > 300.0f && rect->linearVelocity.y > 0)  rect->linearVelocity.y *= -1.0f;
        }
    }

    if (rectangles.size() >= 2) {
        Contact contact = checkSATContact(rectangles[0], rectangles[1]);
        
        if (contact.colliding) {
            // Flash red on collision
            rectangles[0]->graphicsItem->setBrush(QBrush(Qt::red));
            rectangles[1]->graphicsItem->setBrush(QBrush(Qt::red));

            resolveCollision(rectangles[0], rectangles[1], contact, 0.85f);
        } else {
            // Reset to distinct identity colors
            rectangles[0]->graphicsItem->setBrush(QBrush(Qt::cyan));
            rectangles[1]->graphicsItem->setBrush(QBrush(QColor(255, 140, 0)));
        }
    }
}

void Window::updateHUD() {
    QString status = QString(
        "Time: %1 s | Status: %2\n"
        "Press [Space] to Pause & Edit Squares"
    )
    .arg(totalTime, 0, 'f', 2)
    .arg(isPaused ? "PAUSED" : "RUNNING");

    hudText->setPlainText(status);
}

void Window::updateGameLoop() {
    const float dt = 1.0f / 60.0f;

    if (!isPaused) {
        totalTime += dt;
        updatePhysics(dt);
    }

    updateHUD();
    scene->update();
}