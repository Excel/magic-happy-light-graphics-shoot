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

typedef Vector2 GridIndex;
typedef Vector3 WorldPoint;

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
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    // Initialization code
    void initializeResources();
    void loadCubeMap();
    void createModels();
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
    void renderTerrain();
    void paintText();

    //Game code
    Vector3 getMouseRay();
    void shootRay();




    /**
      * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
      * Can be used to index into m_terrain or m_normalMap.
      * Returns -1 if the grid coordinate entered is not valid.
      */
    inline int getIndex(const GridIndex &c);
    /**
      * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
      * Can be used to index into m_terrain or m_normalMap.
      * Returns -1 if the grid coordinate entered is not valid.
      */
    inline int getIndex(int row, int column);

    /**
     * Computes the amount to perturb the height of the vertex currently being processed.
     * Feel free to modify this.
     *
     * @param depth The current recursion depth
     */
    double getPerturb(int cur_depth);
    /**
     * Retrieves the position of each neighbor of the given grid coordinate (i.e. all grid
     * coordinates that can be found one unit horizontally, vertically, or diagonally from
     * the specified grid coordinate).
     *
     * @param coordinate The grid coordinate whose neighbors are to be retrieved
     */
    QList<WorldPoint*> getSurroundingVertices(const GridIndex &coordinate);

    // Methods you will have to fill in

    /**
     * You need to fill this in.
     *
     * Subdivides a square by finding the vertices at its corners, the midpoints of each side, and
     * the center (as the algorithm describes). Then recurs on each of the four sub-squares created.
     *
     * @param topLeft The grid coordinate of the top-left corner of the square to subdivide
     * @param bottomRight The grid coordinate of the bottom-right corner of the square to subdivide
     * @param depth The current recursion depth, decreasing as this function recurses deeper. The
     *              function should stop recurring when this value reaches zero.
     */
    void subdivideSquare(GridIndex tlg, GridIndex brg, GLint curDepth);
    /**
     * You need to fill this in.
     *
     * Computes the normal vector of each terrain vertex and stores it in the corresponding vertex.
     */
    void computeNormals();


    // Other Methods

    /**
     * Sets default values for the four corners of the terrain grid and calls subdivideSquare()
     * to begin the terrain generation process. You do not need to modify this function.
     */
    void populateTerrain();
    /**
     * Draws a line at each vertex showing the direction of that vertex's normal. You may find
     * this to be a useful tool if you're having trouble getting the lighting to look right.
     * By default, this function is called in paintGL(), but only renders anything if
     * m_renderNormals is true. You do not need to modify this function.
     */
    void drawNormals();

    //Member variables

    /** Contains the position of the vertex at each grid point in the terrain */
    WorldPoint *m_terrainMap;
    /** Contains the normal vector of the vertex at each grid point in the terrain */
    Vector3 *m_normalMap;
    /** The number of grid points on one side of the square terrain grid. */
    GLint m_gridLength;
    /** The number of recursion levels to use to generate terrain. Can be used as a level-of-detail parameter. */
    GLint m_depth;
    /** Controls the height of the terrain. Large values generate higher peaks and lower valleys. */
    GLfloat m_roughness;
    /** Controls how much heights can vary per recursion depth level. Higher values generate smoother terrain. */
    GLint m_decay;
    /** Whether or not drawNormals() should do anything when called. */
    bool m_renderNormals;


private:
    QTimer m_timer;
    QTime m_clock;
    int m_prevTime;
    int m_startTime;
    int m_spawnTime;
    float m_prevFps, m_fps;
    OrbitCamera m_camera;
    bool m_firstPersonMode, m_mousePressed, m_autofire;
    int m_score;

    Vector2 m_originalMouse;

    World *m_world;

    bool m_showCollision;

    // Resources
    QHash<QString, Model> m_models; // hash map of all shader programs
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
    QHash<QString, QGLFramebufferObject *> m_framebufferObjects; // hash map of all framebuffer objects
    Model m_dragon; // dragon model
    Model m_friend;
    Model m_enemy;
    GLuint m_skybox; // skybox call list ID
    GLuint m_terrain1; //texture ID
    GLuint m_terrain2; //texture ID
    GLuint m_cubeMap; // cubeMap texture ID
    GLuint m_particle; //particle texture ID
    QFont m_font; // font for rendering text
};

#endif // GLWIDGET_H
