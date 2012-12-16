#include "canvas.h"

#ifdef __APPLE__
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

#include <qgl.h>
#include <iostream>

#include <QMouseEvent>
#include <QTimer>
#include <QFile>

#include <cmath>

using namespace std;

// hack to get around windef.h on Windows
#undef near
#undef far

canvas::canvas(QWidget *parent) : QGLWidget(parent), m_fps(30.0f)
{
    //You may want to add code here
    m_renderNormals = false;
    /*m_camera->eye.x = 0.0f, m_camera->eye.y = 20.f, m_camera->eye.z = 10.0f;
    m_camera->center.x = 0.0f, m_camera->center.y = 0.0f, m_camera->center.z = 0.0f;
    m_camera->up.x = 0.0f, m_camera->up.y = 1.0f, m_camera->up.z = 0.0f;
    m_camera->angle = 45.0f, m_camera->near = 1.0f, m_camera->far = 1000.0f;*/

    // Determines how much each vertex gets perturbed. The larger the value, the less pertubration will occur per recursive value
    m_decay = 2;

    // The number of levels of recursion your terrain uses (can be considered the level of detail of your terrain)
    m_depth = 8;

    // The roughness of your terrain. Higher roughnesses entail taller mountains and deeper valleys. Lower values entail small hills and shallow valleys
    m_roughness = 6;

    // Seed a random number, your terrain will change based on what number you seed
    srand(0);

    // Do not change below here
    /*QObject::connect(this, SIGNAL(_mouseDrag(QMouseEvent*)), this, SLOT(mouseDragEvent(QMouseEvent*)));
    setAutoBufferSwap(false);
    setFocusPolicy(Qt::StrongFocus);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(redraw()));
    m_timer->start(1000.0f / m_fps);*/

    // Imagining your terrain as a square grid of points, gridLength is the number of elements on one side, or the length of a side of the grid
    m_gridLength = (1 << m_depth)+1;

    // Knowing the number of elements on a side gives us the total number of elements in the grid
    int terrain_array_size = m_gridLength * m_gridLength;

    // Represents all of our vertices. Picture this as a grid
    m_terrain = new WorldPoint[terrain_array_size];

    // Represents all the normals, one per each vertex
    m_normalMap = new Vector3[terrain_array_size];
}

// Destructor
canvas::~canvas()
{
    if (m_timer)
    {
        delete m_timer;
        m_timer = 0;
    }

    /*if (m_camera)
    {
        delete m_camera;
        m_camera = 0;
    }*/

    delete[] m_terrain;
    delete[] m_normalMap;
}

// You will only need to deal with the next 3 (maybe 4) methods

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
void canvas::subdivideSquare(GridIndex topleft, GridIndex botright, GLint curDepth)
{
    // TL--TM--TR    +---> x
    // |   |   |     |
    // ML--MM--MR    V
    // |   |   |     y
    // BL--BM--BR

    // corner coordinates (in the grid space [x,y])
    GridIndex TL = GridIndex(topleft.x, topleft.y);
    GridIndex TR = GridIndex(botright.x, topleft.y);
    GridIndex BL = GridIndex(topleft.x, botright.y);
    GridIndex BR = GridIndex(botright.x, botright.y);

    // corner vertices on the terrain (in the grid space [x,y,z])
    WorldPoint &vTL = m_terrain[getIndex(TL)];
    WorldPoint &vTR = m_terrain[getIndex(TR)];
    WorldPoint &vBL = m_terrain[getIndex(BL)];
    WorldPoint &vBR = m_terrain[getIndex(BR)];


    int midy = (topleft.y + botright.y)/2;
    int midx = (topleft.x + botright.x)/2;

    GridIndex TM = GridIndex(midx, topleft.y);
    GridIndex ML = GridIndex(topleft.x, midy);
    GridIndex MM = GridIndex(midx, midy);
    GridIndex MR = GridIndex(botright.x, midy);
    GridIndex BM = GridIndex(midx, botright.y);

    WorldPoint &vTM = m_terrain[getIndex(TM)];
    WorldPoint &vML = m_terrain[getIndex(ML)];
    WorldPoint &vMM = m_terrain[getIndex(MM)];
    WorldPoint &vMR = m_terrain[getIndex(MR)];
    WorldPoint &vBM = m_terrain[getIndex(BM)];


    vTM.y = (vTL.y + vTR.y)/2;
    vTM.x = (vTL.x + vBR.x)/2;
    vTM.z = vTL.z;

    vML.y = (vTL.y + vBL.y)/2;
    vML.x = vTL.x;
    vML.z = (vTL.z + vBR.z)/2;

    vMR.y = (vTR.y + vBR.y)/2;
    vMR.x = vBR.x;
    vMR.z = (vTL.z + vBR.z)/2;

    vBM.y = (vBL.y + vBR.y)/2;
    vBM.x = (vTL.x + vBR.x)/2;
    vBM.z = vBR.z;

    vMM.y = (vTL.y + vTR.y + vBL.y + vBR.y)/4 + getPerturb(curDepth);
    vMM.x = (vTL.x + vBR.x)/2;
    vMM.z = (vTL.z + vBR.z)/2;

    if(curDepth - 1 > 0)
    {
        subdivideSquare(TL, MM, curDepth - 1);
        subdivideSquare(TM, MR, curDepth - 1);
        subdivideSquare(ML, BM, curDepth - 1);
        subdivideSquare(MM, BR, curDepth - 1);
    }



    //
    // @TODO: [Lab 5] Above, we demonstrate how to find
    //                  a) the coordinates in the grid of the corner vertices (in grid space)
    //                  b) the actual (X,Y,Z) points in the terrain map
    //
    //         Now, you just need to compute the midpoints in grid space, average the
    //         heights of the neighboring vertices, and write the new points to the
    //         terrain map. Remember to perturb the new center vertex by a random amount.
    //         Scale this perturbation by curDepth so that the perturbation factor is less
    //         as you recur deeper.
    //
    //         Remember that [X,Y] grid points map to [X,Z] in world space -- the Y
    //         coordinate in world space will be used for the height.
    //
    //         Once you compute these remaining 5 points, you will need to recur on the
    //         four sub-squares you've just created. To do this, you'll call subdivideSquare
    //         recursively, decrementing curDepth by one.
}

/**
 * You need to fill this in.
 *
 * Computes the normal vector of each terrain vertex and stores it in the corresponding vertex.
 */
void canvas::computeNormals()
{
    // For each vertex in the 2D grid...
    for (int row = 0; row < m_gridLength; row++)
    {
        for (int column = 0; column < m_gridLength; column++)
        {
            const GridIndex gridPosition(row, column);                // 2D coordinate of the vertex on the terrain grid
            const int terrainIndex = getIndex(gridPosition);          // Index into the 1D position and normal arrays
            const WorldPoint& vertexPosition  = m_terrain[terrainIndex]; // Position of the vertex

            // Get the neighbors of the vertex at (a,b)
            const QList<WorldPoint*>& neighbors = getSurroundingVertices(gridPosition);
            int numNeighbors = neighbors.size();

            // @TODO: [Lab 5] Compute a list of vectors from vertexPosition to each neighbor in neighbors
            Vector3 *offsets = new Vector3[numNeighbors];
            for (int i = 0; i < numNeighbors; ++i)
                offsets[i] = Vector3(neighbors[i]->x - vertexPosition.z, neighbors[i]->y - vertexPosition.y, neighbors[i]->z - vertexPosition.x); // TODO

            // @TODO: [Lab 5] Compute cross products for each neighbor
            Vector3 *normals = new Vector3[numNeighbors];
            for (int i = 0; i < numNeighbors; ++i)
            {
                int q = (i+1)%numNeighbors;
                normals[i] = neighbors[i]->cross(Vector3(neighbors[q]->x, neighbors[q]->y, neighbors[q]->z)); // TODO
                normals[i] = Vector3(neighbors[i]->z, neighbors[i]->y, neighbors[i]->x).cross(Vector3(neighbors[q]->z, neighbors[q]->y, neighbors[q]->x));
            }

            // Average the normals and store the final value in the normal map
            Vector3 sum = Vector3(0., 0., 0.);
            for (int i = 0; i < numNeighbors; ++i)
                sum += normals[i];
            sum.normalize();
            m_normalMap[terrainIndex] = sum;

            delete[] offsets;
            delete[] normals;
        }
    }
}

/**
 * You need to fill this in.
 *
 * Renders the terrain to the canvas. This is where you should make all of your OpenGL calls.
 */
void canvas::paintGL()
{
    // Clear the color and depth buffers to the current glClearColor
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Push a new matrix onto the stack for modelling transformations
    glPushMatrix();

    // @TODO [Lab5] Tesselate your terrain here.

    loadTexture("/course/cs123/data/BoneHead.jpg");


    for (int row = 0; row < m_gridLength - 1; row++)
    {
        glBegin(GL_TRIANGLE_STRIP);

        for (int column = 0; column < m_gridLength; column++)
        {

            const GridIndex gridPosition(row, column);                // 2D coordinate of the vertex on the terrain grid
            const int terrainIndex = getIndex(gridPosition);          // Index into the 1D position and normal arrays
            const WorldPoint& vertexPosition  = m_terrain[terrainIndex]; // Position of the vertex

            const GridIndex gridPosition2(row + 1, column);                // 2D coordinate of the vertex on the terrain grid
            const int terrainIndex2 = getIndex(gridPosition2);          // Index into the 1D position and normal arrays
            const WorldPoint& vertexPosition2  = m_terrain[terrainIndex2]; // Position of the vertex

            glNormal3dv((GLdouble*)m_normalMap[terrainIndex2].xyz);
            glTexCoord2f((float)(row + 1)/m_gridLength, (float)column/m_gridLength);
            glVertex3dv((GLdouble*)vertexPosition2.xyz);

            glNormal3dv((GLdouble*)m_normalMap[terrainIndex].xyz);
            glTexCoord2f((float)row/m_gridLength, (float)column/m_gridLength);
            glVertex3dv((GLdouble*)vertexPosition.xyz);

        }
        glEnd();
    }

    //divide index by gridsize

    //draw normals
    drawNormals();
    // Discard the modelling transformations (leaving only camera settings)
    glPopMatrix();
    // Force OpenGL to perform all pending operations -- usually a good idea to call this
    glFlush();
    // Swap the buffers to show what we have just drawn onto the screen
    swapBuffers();
}

/**
 * Computes the amount to perturb the height of the vertex currently being processed.
 * Feel free to modify this.
 *
 * @param depth The current recursion depth
 */
double canvas::getPerturb(int cur_depth)
{
    return m_roughness
           * pow((double)cur_depth / m_depth, m_decay)
           * ((rand() % 200-100) / 100.0);
}

/****************************************************************************************************************/
/**********************************DO NOT MODIFY ANY OF THE FOLLOWING CODE***************************************/
/****************************************************************************************************************/
/**
 * Sets default values for the four corners of the terrain grid and calls subdivideSquare()
 * to begin the terrain generation process. You do not need to modify this function.
 */
void canvas::populateTerrain()
{
    WorldPoint tl(-10, 0, -10);
    WorldPoint tr(10, 0, -10);
    WorldPoint bl(-10, 0, 10);
    WorldPoint br(10, 0, 10);
    GridIndex tlg(0,0);
    GridIndex trg(0,m_gridLength-1);
    GridIndex blg(m_gridLength-1, 0);
    GridIndex brg(m_gridLength-1, m_gridLength-1);
    m_terrain[getIndex(tlg)] = tl;
    m_terrain[getIndex(trg)] = tr;
    m_terrain[getIndex(blg)] = bl;
    m_terrain[getIndex(brg)] = br;
    subdivideSquare(tlg, brg, m_depth);
}

/**
  * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
  * Can be used to index into m_terrain or m_normalMap.
  * Returns -1 if the grid coordinate entered is not valid.
  */
inline int canvas::getIndex(const GridIndex &c)
{
    return getIndex(c.x, c.y);
}

/**
  * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
  * Can be used to index into m_terrain or m_normalMap.
  * Returns -1 if the grid coordinate entered is not valid.
  */
inline int canvas::getIndex(int row, int col)
{
    if (row < 0 || row >= m_gridLength || col < 0 || col >= m_gridLength)
        return -1;

    return row * m_gridLength + col;
}

// All of the following methods are to be used in finding the normals

/**
 * Retrieves the position of each neighbor of the given grid coordinate (i.e. all grid
 * coordinates that can be found one unit horizontally, vertically, or diagonally from
 * the specified grid coordinate).
 *
 * @param coordinate The grid coordinate whose neighbors are to be retrieved
 */
QList<WorldPoint*> canvas::getSurroundingVertices(const GridIndex &coordinate)
{
    GridIndex coords[8];
    coords[0] = GridIndex(coordinate.x,     coordinate.y - 1);
    coords[1] = GridIndex(coordinate.x + 1, coordinate.y - 1);
    coords[2] = GridIndex(coordinate.x + 1, coordinate.y);
    coords[3] = GridIndex(coordinate.x + 1, coordinate.y + 1);
    coords[4] = GridIndex(coordinate.x,     coordinate.y + 1);
    coords[5] = GridIndex(coordinate.x - 1, coordinate.y + 1);
    coords[6] = GridIndex(coordinate.x - 1, coordinate.y);
    coords[7] = GridIndex(coordinate.x - 1, coordinate.y - 1);

    int index;
    QList<WorldPoint*> vecs;

    for (int i = 0; i < 8; i++)
    {
        index = getIndex(coords[i]);
        if (index != -1)
            vecs.push_back(& m_terrain[index]);
    }

    return vecs;
}

/**
 * Draws a line at each vertex showing the direction of that vertex's normal. You may find
 * this to be a useful tool if you're having trouble getting the lighting to look right.
 * By default, this function is called in paintGL(), but only renders anything if
 * m_renderNormals is true. You do not need to modify this function.
 */
void canvas::drawNormals()
{
    if (m_renderNormals)
    {
        glColor3f(1,0,0);

        for (int row = 0; row < m_gridLength; row++)
        {
            for (int column = 0; column < m_gridLength; column++)
            {
                glBegin(GL_LINES);

                WorldPoint curVert = m_terrain[getIndex(row, column)];
                Vector3 curNorm = m_normalMap[getIndex(row, column)];

                glNormal3dv((GLdouble*)curNorm.xyz);
                glVertex3f(curVert.x, curVert.y, curVert.z);
                glVertex3f(curVert.x +curNorm.x,
                           curVert.y + curNorm.y,
                           curVert.z + curNorm.z);

                glEnd();
            }
        }
    }
}

/** Sets the initial values of OpenGL state variables used by this lab */
/*void Lab5Canvas::initializeGL()
{
    glClearColor(0, 0, 0, 0);   // Always reset the screen to black before drawing anything
    glEnable(GL_DEPTH_TEST);    // When drawing a triangle, only keep pixels closer to the camera than what's already been drawn

    // Make things pretty
    glEnable(GL_MULTISAMPLE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Bind the ambient and diffuse color of each vertex to the current glColor() value
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Cull triangles that are facing away from the camera
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Set up a single light
    glEnable(GL_LIGHTING);
    GLfloat ambientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuseColor[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat specularColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat lightPosition[] = { 0.f, 0.f, 10.f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);

    // Enable texturing - feel free to substitue with your own image
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if (loadTexture("/course/cs123/data/image/BoneHead.jpg") == -1)
        cout << "Texture does not exist" << endl;

    populateTerrain();
    computeNormals();
}*/

/** Updates the current OpenGL state to avoid object distortion when the window is resized. */
/*void Lab5Canvas::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    // Set the viewport to be the entire window (you probably want to keep this)
    glViewport(0, 0, w, h);
    updateCamera();
}*/









void canvas::redraw()
{
    repaint();
}

void canvas::settingsChanged() { }

/**
 * Generates a new OpenGL texture and loads the image data from the file at the given path into
 * the texture. Makes the new texture active and returns its unique texture ID.
 */
GLuint canvas::loadTexture(const QString &filename)
{
    // Make sure the image file exists
    QFile file(filename);
    if (!file.exists())
        return -1;

    // Load the file into memory
    QImage image;
    image.load(file.fileName());
    image = image.mirrored(false, true);
    QImage texture = QGLWidget::convertToGLFormat(image);

    // Generate a new OpenGL texture ID to put our image into
    GLuint id = 0;
    glGenTextures(1, &id);

    // Make the texture we just created the new active texture
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);

    // Copy the image data into the OpenGL texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

    // Set filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set coordinate wrapping options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return id;
}
