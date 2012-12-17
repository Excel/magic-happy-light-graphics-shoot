#include "glwidget.h"

#include <iostream>
#include <QFileDialog>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QMouseEvent>
#include <QTime>
#include <QTimer>
#include <QWheelEvent>
#include "glm.h"


#include "game/World.h"
#include "game/Target.h"

using std::cout;
using std::endl;

extern "C"
{
    extern void APIENTRY glActiveTexture(GLenum);
}

static const int MAX_FPS = 120;

/**
  Constructor.  Initialize all member variables here.
 **/
GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent),
    m_timer(this), m_prevTime(0), m_spawnTime(0), m_prevFps(0.f), m_fps(0.f),
    m_font("Deja Vu Sans Mono", 8, 4)
{

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_camera.center = Vector3(0.f, 0.f, 0.f);
    m_camera.up = Vector3(0.f, 1.f, 0.f);
    m_camera.zoom = 3.5f;
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));

    m_originalMouse = Vector2(-1, -1);
    m_firstPersonMode = false;

    m_autofire = false;
    m_mousePressed = false;

    m_world = new World();
    m_showCollision = false;

    setCursor(Qt::ArrowCursor);




    /** terrain stuffs */
    //You may want to add code here
    m_renderNormals = false;

    // Determines how much each vertex gets perturbed. The larger the value, the less pertubration will occur per recursive value
    m_decay = 3;

    // The number of levels of recursion your terrain uses (can be considered the level of detail of your terrain)
    m_depth = 8;

    // The roughness of your terrain. Higher roughnesses entail taller mountains and deeper valleys. Lower values entail small hills and shallow valleys
    m_roughness = 6;

    // Seed a random number, your terrain will change based on what number you seed
    srand(3);

    // Imagining your terrain as a square grid of points, gridLength is the number of elements on one side, or the length of a side of the grid
    m_gridLength = (1 << m_depth)+1;

    // Knowing the number of elements on a side gives us the total number of elements in the grid
    int terrain_array_size = m_gridLength * m_gridLength;

    // Represents all of our vertices. Picture this as a grid
    m_terrainMap = new WorldPoint[terrain_array_size];

    // Represents all the normals, one per each vertex
    m_normalMap = new Vector3[terrain_array_size];

    populateTerrain();
    computeNormals();


}

/**
  Destructor.  Delete any 'new'ed objects here.
 **/
GLWidget::~GLWidget()
{
    foreach (QGLShaderProgram *sp, m_shaderPrograms)
        delete sp;
    foreach (QGLFramebufferObject *fbo, m_framebufferObjects)
        delete fbo;
    glDeleteLists(m_skybox, 1);
    const_cast<QGLContext *>(context())->deleteTexture(m_cubeMap);
    glmDelete(m_dragon.model);
    glmDelete(m_friend.model);
    glmDelete(m_enemy.model);

    delete m_world;


    delete[] m_terrainMap;
    delete[] m_normalMap;
}

/**
  Initialize the OpenGL state and start the drawing loop.
 **/
void GLWidget::initializeGL()
{

    // Set up OpenGL
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glDisable(GL_DITHER);

    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClear(GL_ACCUM_BUFFER_BIT);


    // Load resources, including creating shader programs and framebuffer objects
    initializeResources();

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);

    m_startTime = m_clock.elapsed();

    //emit _glInit(); //leave this

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
    glShadeModel(GL_SMOOTH);
    GLfloat ambientColor[] = { 0.55f, 0.3f, 0.55f, 1.0f };
    GLfloat diffuseColor[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat specularColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat lightPosition[] = { 0.f, 0.f, 10.f, 0.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_LIGHT0);


    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    m_terrain1 = loadTexture(":/textures/terrain/sand-texture.jpg");
    m_terrain2 = loadTexture(":/textures/terrain/sand-texture.jpg");
    if (m_terrain1 == (unsigned int)(-1))
        cout << "Texture chris1 does not exist" << endl;
    if (m_terrain2 == (unsigned int)(-1))
        cout << "Texture does not exist" << endl;

}

/**
  Initialize all resources.
  This includes models, textures, call lists, shader programs, and framebuffer objects.
 **/
void GLWidget::initializeResources()
{
    cout << "Using OpenGL Version " << glGetString(GL_VERSION) << endl << endl;
    // Ideally we would now check to make sure all the OGL functions we use are supported
    // by the video card.  But that's a pain to do so we're not going to.
    cout << "--- Loading Resources ---" << endl;

    createModels();
    cout << "Loaded models..." << endl;

    m_skybox = ResourceLoader::loadSkybox();
    cout << "Loaded skybox..." << endl;

    loadCubeMap();
    cout << "Loaded cube map..." << endl;

    createShaderPrograms();
    cout << "Loaded shader programs..." << endl;

    createFramebufferObjects(width(), height());
    cout << "Loaded framebuffer objects..." << endl;

    m_particle = loadTexture(":/textures/particles/particle2.bmp");
    cout << "Loaded particle textures..." << endl;

    cout << " --- Finish Loading Resources ---" << endl;

}

/**
  Load a cube map for the skybox
 **/
void GLWidget::loadCubeMap()
{
    QList<QFile *> fileList;
    fileList.append(new QFile(":/textures/nebula/purplez_right1.jpg"));
    fileList.append(new QFile(":/textures/nebula/purplez_left2.jpg"));
    fileList.append(new QFile(":/textures/nebula/purplez_top3.jpg"));
    fileList.append(new QFile(":/textures/nebula/purplez_bottom4.jpg"));
    fileList.append(new QFile(":/textures/nebula/purplez_front5.jpg"));
    fileList.append(new QFile(":/textures/nebula/purplez_back6.jpg"));
    m_cubeMap = ResourceLoader::loadCubeMap(fileList);
}

/**
  Create models.
  **/

void GLWidget::createModels()
{
    m_dragon = ResourceLoader::loadObjModel("/home/jqtran/course/cs123_final/models/xyzrgb_dragon.obj");
    m_friend = ResourceLoader::loadObjModel("/home/jqtran/course/cs123_final/models/Gargoyle_1.obj");
    m_enemy = ResourceLoader::loadObjModel("/home/jqtran/course/cs123_final/models/KinjaDragern.obj");

    m_models["dragon"] = m_dragon;
    m_models["friend"] = m_friend;
    m_models["enemy"] = m_enemy;
}

/**
  Create shader programs.
 **/
void GLWidget::createShaderPrograms()
{
    const QGLContext *ctx = context();
    m_shaderPrograms["reflect"] = ResourceLoader::newShaderProgram(ctx, ":/shaders/reflect.vert", ":/shaders/reflect.frag");
    m_shaderPrograms["refract"] = ResourceLoader::newShaderProgram(ctx, ":/shaders/refract.vert", ":/shaders/refract.frag");
    m_shaderPrograms["brightpass"] = ResourceLoader::newFragShaderProgram(ctx, ":/shaders/brightpass.frag");
    m_shaderPrograms["blur"] = ResourceLoader::newFragShaderProgram(ctx, ":/shaders/blur.frag");
    m_shaderPrograms["psycho"] = ResourceLoader::newShaderProgram(ctx, ":/shaders/psycho.vert", ":/shaders/psycho.frag");
}

/**
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void GLWidget::createFramebufferObjects(int width, int height)
{
    // Allocate the main framebuffer object for rendering the scene to
    // This needs a depth attachment
    m_framebufferObjects["fbo_0"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_framebufferObjects["fbo_0"]->format().setSamples(16);
    // Allocate the secondary framebuffer obejcts for rendering textures to (post process effects)
    // These do not require depth attachments
    m_framebufferObjects["fbo_1"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::NoAttachment,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);

    m_framebufferObjects["fbo_2"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::NoAttachment,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
}

/**
  Called to switch to an orthogonal OpenGL camera.
  Useful for rending a textured quad across the whole screen.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::applyOrthogonalCamera(float width, float height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.f, width, 0.f, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  Called to switch to a perspective OpenGL camera.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::applyPerspectiveCamera(float width, float height)
{
    float ratio = ((float) width) / height;
    Vector3 dir(-Vector3::fromAngles(m_camera.theta, m_camera.phi));
    Vector3 eye(m_camera.center - dir * m_camera.zoom);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera.fovy, ratio, 0.1f, 1000.f);
    gluLookAt(eye.x, eye.y, eye.z, eye.x + dir.x, eye.y + dir.y, eye.z + dir.z,
              m_camera.up.x, m_camera.up.y, m_camera.up.z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  Draws the scene to a buffer which is rendered to the screen when this function exits.
 **/
void GLWidget::paintGL()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the fps
    int time = m_clock.elapsed();
    m_fps = 1000.f / (time - m_prevTime);
    m_prevTime = time;
    int width = this->width();
    int height = this->height();

    // Render the scene to a framebuffer
    m_framebufferObjects["fbo_0"]->bind();
    applyPerspectiveCamera(width, height);
    renderScene();

    m_framebufferObjects["fbo_0"]->release();

    // Copy the rendered scene into framebuffer 1
    m_framebufferObjects["fbo_0"]->blitFramebuffer(m_framebufferObjects["fbo_1"],
                                                   QRect(0, 0, width, height), m_framebufferObjects["fbo_0"],
                                                   QRect(0, 0, width, height), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // TODO: Step 0 - draw the scene to the screen as a textured quad

    applyOrthogonalCamera(width, height);
    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
    renderTexturedQuad(width, height);
    glBindTexture(GL_TEXTURE_2D, 0);

    paintText();
}


/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void GLWidget::renderScene()
{
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Enable cube maps and draw the skybox
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
    glCallList(m_skybox);

    // Enable culling (back) faces for rendering the dragon
    glEnable(GL_CULL_FACE);

    // Render the dragon with the refraction shader bound
    glActiveTexture(GL_TEXTURE0);

    int time = m_clock.elapsed();

    /*!
      just make new targets at the moment until bezier curves work
      */
    if(m_firstPersonMode){
        if((time - m_spawnTime) >= 1000){
            int numTargets =  rand() %3 + 1;
            for(int i = 0; i < numTargets; i++){
                bool frand = rand () % 2;
                cout<<frand<<"\n";
                Target* t = new Target(m_camera.center + Vector3(rand() % 10 - 5.0f, rand() % 10 - 5.0f, rand() % 10 - 5.0f), Vector2(0.f, 0.f), m_particle, frand ? m_friend : m_enemy, frand);
                t->setWorld(m_world);
                m_world->addTarget(t);
                m_spawnTime = time;
            }

        }
    }

    /**
      for all entities (bullets, targets), draw them
      */
    for(int i = 0; i < m_world->getEntities().size(); i++){
        Entity* e = m_world->getEntities().at(i);


        // Render the dragon with the shader specified by the entity
        m_shaderPrograms[e->getShader()]->bind();
        m_shaderPrograms[e->getShader()]->setUniformValue("CubeMap", GL_TEXTURE0);
        m_shaderPrograms["psycho"]->setUniformValue("time", (m_clock.elapsed() - m_startTime)/500.f);
        glPushMatrix();

        //translate by the position and rotate by theta and phi
        Vector3 targetPos = e->getPos();
        glTranslatef(targetPos.x, targetPos.y, targetPos.z);
        glRotatef(e->getRotation().x, 0.0f, 1.0f, 0.0f);
        glRotatef(e->getRotation().y, 0.0f, 0.0f, 1.0f);

        //draw the model
        glCallList(m_models[e->getModel()].idx);
        glPopMatrix();
        m_shaderPrograms[e->getShader()]->release();

        //draw the collision sphere
        if(m_showCollision){
            e->onCollisionRender();
        }

        //attempt to draw the particle emitters
       e->onRender();

    }

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);

    renderTerrain();

    glDisable(GL_DEPTH_TEST);
    //update the entities' positions/collisions/whatnot
    if(m_firstPersonMode){
        m_world->onUpdate();

        m_camera.center = m_world->getPathPoint();

        //shoot ray if mouse held down and if this is a good idea
        if(m_mousePressed && m_autofire){
            shootRay();
        }
    }
}


void GLWidget::renderTerrain(){
    glBindTexture(GL_TEXTURE_2D, m_terrain2);
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Draw the  quad
//    glBegin(GL_QUADS);
//    glTexCoord2f(0.0f, 0.0f);
//    glVertex3f(-1*100*1.0f, -90.0f, -1*100*1.0f);
//    glTexCoord2f(1.0f, 0.0f);
//    glVertex3f(100*1.0f, -90.0f, -1*100*1.0f);
//    glTexCoord2f(1.0f, 1.0f);
//    glVertex3f(100*1.0f, -90.0f, 100*1.0f);
//    glTexCoord2f(0.0f, 1.0f);
//    glVertex3f(-1*100*1.0f, -90.0f, 100*1.0f);
//    glEnd();


    // Push a new matrix onto the stack for modelling transformations
    glPushMatrix();

    // @TODO [Lab5] Tesselate your terrain here.


    //glFrontFace(GL_CW);
    for (int row = 0; row < m_gridLength-1; row++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int column = 0; column < m_gridLength; column++)
        {
            WorldPoint curVert = m_terrainMap[getIndex(row, column)];
            Vector3 curNorm = m_normalMap[getIndex(row, column)];


            WorldPoint curVert1 = m_terrainMap[getIndex(row+1, column)];
            Vector3 curNorm1 = m_normalMap[getIndex(row+1, column)];

            if(curVert1.y > -70.0f){
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindTexture(GL_TEXTURE_2D, m_terrain1);

            }

            glNormal3fv(curNorm1.xyz);
            glTexCoord2f(1.f * row/m_gridLength, 1.f *column/m_gridLength);
            glVertex3fv(curVert1.xyz);
            glNormal3fv(curNorm.xyz);
            glTexCoord2f(1.f * (row+1)/m_gridLength, 1.f *column/m_gridLength);
            glVertex3fv(curVert.xyz);
            //cout<<curVert<<" "<<curVert1<<" "<<row <<" "<<column<< "\n";
        }
        glEnd();
    }



    //draw normals
    //drawNormals();
    // Discard the modelling transformations (leaving only camera settings)
    glPopMatrix();
    // Force OpenGL to perform all pending operations -- usually a good idea to call this
    glFlush();
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
  Called when the mouse is dragged.  Rotates the camera based on mouse movement.
  Centers the mouse to the middle of the screen.
**/
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

    if(m_firstPersonMode) {
        if(m_originalMouse.x < 0 && m_originalMouse.y < 0)
        {
            m_originalMouse = Vector2(event->globalX() - event->x() + this->width()/2, event->globalY() - event->y() + this->height()/2);
            QCursor::setPos(m_originalMouse.x, m_originalMouse.y);
        }
        else
        {
            Vector2 pos(event->globalX(), event->globalY());
            m_camera.mouseMove(pos - m_originalMouse);

            QCursor::setPos(m_originalMouse.x, m_originalMouse.y);
        }
        update();
    }

}

/**
  Record a mouse press position. Shoots a ray if the game is not paused.
 **/
void GLWidget::mousePressEvent(QMouseEvent *event)
{

    if(!m_firstPersonMode){
        setCursor(Qt::CrossCursor);
        m_firstPersonMode = true;

        m_originalMouse = Vector2(event->globalX() - event->x() + this->width()/2, event->globalY() - event->y() + this->height()/2);
        QCursor::setPos(m_originalMouse.x, m_originalMouse.y);

    }
    else {
        //shoots a bullet
        m_originalMouse = Vector2(event->globalX(), event->globalY());

        shootRay();

        update();
    }

}

/**
  Called when the mouse is released. Helps with turning off autofire.
 **/
void GLWidget::mouseReleaseEvent(QMouseEvent *event){
    m_mousePressed = false;
    m_originalMouse = Vector2(event->globalX() - event->x() + this->width()/2, event->globalY() - event->y() + this->height()/2);

}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera.mouseWheel(event->delta());
    }
}

/**
  Called when the GLWidget is resized.
 **/
void GLWidget::resizeGL(int width, int height)
{
    // Resize the viewport
    glViewport(0, 0, width, height);

    // Reallocate the framebuffers with the new window dimensions
    foreach (QGLFramebufferObject *fbo, m_framebufferObjects)
    {
        const QString &key = m_framebufferObjects.key(fbo);
        QGLFramebufferObjectFormat format = fbo->format();
        delete fbo;
        m_framebufferObjects[key] = new QGLFramebufferObject(width, height, format);
    }
}

/**
  Draws a textured quad. The texture must be bound and unbound
  before and after calling this method - this method assumes that the texture
  has been bound beforehand.

  @param w: the width of the quad to draw
  @param h: the height of the quad to draw
**/
void GLWidget::renderTexturedQuad(int width, int height) {
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Draw the  quad
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(width, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(width, height);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, height);
    glEnd();
}

/**
  Creates a gaussian blur kernel with the specified radius.  The kernel values
  and offsets are stored.

  @param radius: The radius of the kernel to create.
  @param width: The width of the image.
  @param height: The height of the image.
  @param kernel: The array to write the kernel values to.
  @param offsets: The array to write the offset values to.
**/
void GLWidget::createBlurKernel(int radius, int width, int height,
                                                    GLfloat* kernel, GLfloat* offsets)
{
    int size = radius * 2 + 1;
    float sigma = radius / 3.0f;
    float twoSigmaSigma = 2.0f * sigma * sigma;
    float rootSigma = sqrt(twoSigmaSigma * M_PI);
    float total = 0.0f;
    float xOff = 1.0f / width, yOff = 1.0f / height;
    int offsetIndex = 0;
    for (int y = -radius, idx = 0; y <= radius; ++y)
    {
        for (int x = -radius; x <= radius; ++x,++idx)
        {
            float d = x * x + y * y;
            kernel[idx] = exp(-d / twoSigmaSigma) / rootSigma;
            total += kernel[idx];
            offsets[offsetIndex++] = x * xOff;
            offsets[offsetIndex++] = y * yOff;
        }
    }
    for (int i = 0; i < size * size; ++i)
    {
        kernel[i] /= total;
    }
}

/**
  Handles any key press from the keyboard
 **/
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_S){
        QImage qi = grabFrameBuffer(false);
        QString filter;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png)"), &filter);
        qi.save(QFileInfo(fileName).absoluteDir().absolutePath() + "/" + QFileInfo(fileName).baseName() + ".png", "PNG", 100);
    }
    else if(event->key() == Qt::Key_C){
        //toggle collision display
        m_showCollision = !m_showCollision;
    }
    else if(event->key() == Qt::Key_Escape){
        //pause the game
        setCursor(Qt::ArrowCursor);
        m_firstPersonMode = false;
        m_originalMouse = Vector2(-1, -1);
    }
}

/**
  Draws text for the FPS and screenshot prompt
 **/
void GLWidget::paintText()
{
    glColor3f(1.f, 1.f, 1.f);

    // Combine the previous and current framerate
    if (m_fps >= 0 && m_fps < 1000)
    {
       m_prevFps *= 0.95f;
       m_prevFps += m_fps * 0.05f;
    }

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
    renderText(10, 35, "Score: " + QString::number((int) (m_world->getScore())), m_font);
    renderText(10,50, "Esc: Pause", m_font);
    if(m_showCollision){
        renderText(10, 65, "C: Turn OFF collision detection display", m_font);
    }
    else{
        renderText(10, 65, "C: Turn ON collision detection display", m_font);
    }
    renderText(10, 80, "S: Save screenshot", m_font);
}

Vector3 GLWidget::getMouseRay()
{
    //get the direction of the center to the mouse point
    Vector3 dir(-Vector3::fromAngles(m_camera.theta, m_camera.phi));
    //Vector3 eye(m_camera.center - dir * m_camera.zoom);

    Vector3 ray = dir;
    ray.normalize();
    return ray;
}
void GLWidget::shootRay(){
    //fire a bullet
    Vector3 ray = getMouseRay();

    m_mousePressed = true;
    m_world->fireRay(m_camera.center, ray, m_camera, m_particle, m_dragon);
}

/**
  * Loads the image at the given path and copies its data into an OpenGL texture.
  * @return The unique ID of the texture generated by this function.
  */
GLuint GLWidget::loadTexture(const QString &path)
{
    QFile file(path);

    QImage image, texture;
    if(!file.exists()) return -1;
    image.load(file.fileName());
    texture = QGLWidget::convertToGLFormat(image);

    glEnable(GL_TEXTURE_2D);

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    return id;
}













void GLWidget::applyShaders(){
    int width = this->width();
    int height = this->height();
    // TODO: Step 1 - use the brightpass shader to render bright areas
    // only to fbo_2
    m_framebufferObjects["fbo_2"]->bind();
    m_shaderPrograms["brightpass"]->bind();
    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
    //draw

    renderTexturedQuad(width, height);
    m_shaderPrograms["brightpass"]->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_framebufferObjects["fbo_2"]->release();

    // TODO: Uncomment this section in step 2 of the lab
     float scales[] = {4.f,8.f};
    for (int i = 0; i < 2; ++i)
    {
        // Render the blurred brightpass filter result to fbo 1
       renderBlur(width / scales[i], height / scales[i]);

       // Bind the image from fbo to a texture
        glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Enable alpha blending and render the texture to the screen
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        renderTexturedQuad(width * scales[i], height * scales[i]);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


/**
  Run a gaussian blur on the texture stored in fbo 2 and
  put the result in fbo 1.  The blur should have a radius of 2.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::renderBlur(int width, int height)
{
    int radius = 2;
    int dim = radius * 2 + 1;
    GLfloat kernel[dim * dim];
    GLfloat offsets[dim * dim * 2];
    createBlurKernel(radius, width, height, &kernel[0], &offsets[0]);

    // TODO: Step 2 - Finish filling this in
    m_framebufferObjects["fbo_1"]->bind();
    m_shaderPrograms["blur"]->bind();
    glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_2"]->texture());
    //draw


    m_shaderPrograms["blur"]->setUniformValue("arraySize", dim*dim);
    m_shaderPrograms["blur"]->setUniformValueArray("offsets", offsets, dim*dim*2, 2);
    m_shaderPrograms["blur"]->setUniformValueArray("kernel", kernel, dim*dim, 1);

    renderTexturedQuad(width, height);
    m_shaderPrograms["blur"]->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_framebufferObjects["fbo_1"]->release();

}

/** Terrain stuffs */
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
void GLWidget::subdivideSquare(GridIndex topleft, GridIndex botright, GLint curDepth)
{
    // TL--TM--TR    +---> x
    // |   |   |     |
    // ML--MM--MR    V
    // |   |   |     y
    // BL--BM--BR

    if(curDepth == 0){
        return;
    }

    GridIndex TL = GridIndex(topleft.x, topleft.y);
    GridIndex TR = GridIndex(botright.x, topleft.y);
    GridIndex BL = GridIndex(topleft.x, botright.y);
    GridIndex BR = GridIndex(botright.x, botright.y);

    // corner vertices on the terrain (in the grid space [x,y,z])
    WorldPoint &vTL = m_terrainMap[getIndex(TL)];
    WorldPoint &vTR = m_terrainMap[getIndex(TR)];
    WorldPoint &vBL = m_terrainMap[getIndex(BL)];
    WorldPoint &vBR = m_terrainMap[getIndex(BR)];

    GridIndex TM = GridIndex((TR.x + TL.x)/2, TL.y);
    GridIndex BM = GridIndex((BR.x + BL.x)/2, BL.y);
    GridIndex RM = GridIndex(TR.x, (BR.y + TR.y)/2);
    GridIndex LM = GridIndex(TL.x, (BL.y + TL.y)/2);

    double midx = (vTL.x + vBR.x)/2;
    double midy = (vTL.z + vBR.z)/2;

    WorldPoint &vTM = m_terrainMap[getIndex(TM)];
    vTM.x = midx;
    vTM.y = (vTL.y + vTR.y)/2;
    vTM.z = vTL.z;

    WorldPoint &vBM = m_terrainMap[getIndex(BM)];
    vBM.x = midx;
    vBM.y = (vBL.y + vBR.y)/2;
    vBM.z = vBR.z;

    WorldPoint &vRM = m_terrainMap[getIndex(RM)];
    vRM.x = vBR.x;
    vRM.y = (vTR.y + vBR.y)/2;
    vRM.z = midy;

    WorldPoint &vLM = m_terrainMap[getIndex(LM)];
    vLM.x = vTL.x;
    vLM.y = (vTL.y + vBL.y)/2;
    vLM.z = midy;

    GridIndex MP = GridIndex((TR.x + TL.x)/2, (BL.y + TL.y)/2);
    WorldPoint &vMP = m_terrainMap[getIndex(MP)];
    vMP.x = midx;
    vMP.z = midy;
    vMP.y = (vTL.y + vTR.y + vBL.y + vBR.y)/4 + getPerturb(curDepth);

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

    if(curDepth != 0){

        subdivideSquare(TL, MP, curDepth-1);
        subdivideSquare(TM, RM, curDepth-1);
        subdivideSquare(LM, BM, curDepth-1);
        subdivideSquare(MP, BR, curDepth-1);
    }
}

/**
 * You need to fill this in.
 *
 * Computes the normal vector of each terrain vertex and stores it in the corresponding vertex.
 */
void GLWidget::computeNormals()
{
    // For each vertex in the 2D grid...
    for (int row = 0; row < m_gridLength; row++)
    {
        for (int column = 0; column < m_gridLength; column++)
        {
            const GridIndex gridPosition(row, column);                // 2D coordinate of the vertex on the terrain grid
            const int terrainIndex = getIndex(gridPosition);          // Index into the 1D position and normal arrays
            const WorldPoint& vertexPosition  = m_terrainMap[terrainIndex]; // Position of the vertex

            // Get the neighbors of the vertex at (a,b)
            const QList<WorldPoint*>& neighbors = getSurroundingVertices(gridPosition);
            int numNeighbors = neighbors.size();

            // @TODO: [Lab 5] Compute a list of vectors from vertexPosition to each neighbor in neighbors
            Vector3 *offsets = new Vector3[numNeighbors];
            for (int i = 0; i < numNeighbors; ++i){
                offsets[i].x = neighbors[i]->x - vertexPosition.x;
                offsets[i].y = neighbors[i]->y - vertexPosition.y;
                offsets[i].z = neighbors[i]->z - vertexPosition.z;            ; // TODO
            }
            // @TODO: [Lab 5] Compute cross products for each neighbor
            Vector3 *normals = new Vector3[numNeighbors];
            for (int i = 0; i < numNeighbors; ++i){
                Vector3 offset = Vector3(offsets[i].z, offsets[i].y, offsets[i].x);
                Vector3 offset1 = Vector3(offsets[(i+1)%numNeighbors].z, offsets[(i+1)%numNeighbors].y, offsets[(i+1)%numNeighbors].x);
                normals[i] = offset.cross(offset1);
               //Vector3::zero(); // TODO
            }
            // Average the normals and store the final value in the normal map
            Vector3 sum(0, 0, 0);
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
 * Computes the amount to perturb the height of the vertex currently being processed.
 * Feel free to modify this.
 *
 * @param depth The current recursion depth
 */
double GLWidget::getPerturb(int cur_depth)
{
    return m_roughness
           * pow((double)cur_depth / m_depth, m_decay)
           * ((rand() % 200-100) / 10.0);
}

/****************************************************************************************************************/
/**********************************DO NOT MODIFY ANY OF THE FOLLOWING CODE***************************************/
/****************************************************************************************************************/
/**
 * Sets default values for the four corners of the terrain grid and calls subdivideSquare()
 * to begin the terrain generation process. You do not need to modify this function.
 */
void GLWidget::populateTerrain()
{
    WorldPoint tl(-150, -70, -150);
    WorldPoint tr(150, -70, -150);
    WorldPoint bl(-150, -70, 150);
    WorldPoint br(150, -70, 150);
    GridIndex tlg(0,0);
    GridIndex trg(0,m_gridLength-1);
    GridIndex blg(m_gridLength-1, 0);
    GridIndex brg(m_gridLength-1, m_gridLength-1);
    m_terrainMap[getIndex(tlg)] = tl;
    m_terrainMap[getIndex(trg)] = tr;
    m_terrainMap[getIndex(blg)] = bl;
    m_terrainMap[getIndex(brg)] = br;
    subdivideSquare(tlg, brg, m_depth);
}

/**
  * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
  * Can be used to index into m_terrain or m_normalMap.
  * Returns -1 if the grid coordinate entered is not valid.
  */
inline int GLWidget::getIndex(const GridIndex &c)
{
    return getIndex(c.x, c.y);
}

/**
  * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
  * Can be used to index into m_terrain or m_normalMap.
  * Returns -1 if the grid coordinate entered is not valid.
  */
inline int GLWidget::getIndex(int row, int col)
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
QList<WorldPoint*> GLWidget::getSurroundingVertices(const GridIndex &coordinate)
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
            vecs.push_back(& m_terrainMap[index]);
    }

    return vecs;
}

/**
 * Draws a line at each vertex showing the direction of that vertex's normal. You may find
 * this to be a useful tool if you're having trouble getting the lighting to look right.
 * By default, this function is called in paintGL(), but only renders anything if
 * m_renderNormals is true. You do not need to modify this function.
 */
void GLWidget::drawNormals()
{
    if (m_renderNormals)
    {
        glColor3f(1,0,0);

        for (int row = 0; row < m_gridLength; row++)
        {
            for (int column = 0; column < m_gridLength; column++)
            {
                glBegin(GL_LINES);

                WorldPoint curVert = m_terrainMap[getIndex(row, column)];
                Vector3 curNorm = m_normalMap[getIndex(row, column)];

                glNormal3fv(curNorm.xyz);
                glVertex3f(curVert.x, curVert.y, curVert.z);
                glVertex3f(curVert.x +curNorm.x,
                           curVert.y + curNorm.y,
                           curVert.z + curNorm.z);

                glEnd();
            }
        }
    }
}

