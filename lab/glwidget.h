#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QTime>

#include "camera.h"
#include "vector.h"
#include "resourceloader.h"
#include "game/World.h"

class QGLShaderProgram;
class QGLFramebufferObject;


class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

protected:
    // Overridden QGLWidget methods
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    // Initialization code
    void initializeResources();
    void loadCubeMap();
    void createShaderPrograms();
    void createFramebufferObjects(int width, int height);
    void createBlurKernel(int radius, int width, int height, GLfloat* kernel, GLfloat* offsets);
    GLuint loadTexture(const QString &path);

    // Drawing code
    void applyOrthogonalCamera(float width, float height);
    void applyPerspectiveCamera(float width, float height);
    void applyShaders();
    void renderTexturedQuad(int width, int height);
    void renderBlur(int width, int height);
    void renderScene();
    void paintText();

    //Calculating code
    Vector3 getMouseRay();

private:
    QTimer m_timer;
    QTime m_clock;
    int m_prevTime;
    int m_startTime;
    float m_prevFps, m_fps;
    OrbitCamera m_camera;
    bool m_firstPersonMode, m_fired;
    int m_score;

    Vector2 m_originalMouse;


    World *m_world;

    bool m_showCollision;

    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
    QHash<QString, QGLFramebufferObject *> m_framebufferObjects; // hash map of all framebuffer objects
    Model m_dragon; // dragon model
    GLuint m_skybox; // skybox call list ID
    GLuint m_cubeMap; // cubeMap texture ID
    GLuint m_particle; //particle texture ID
    QFont m_font; // font for rendering text

};

#endif // GLWIDGET_H
