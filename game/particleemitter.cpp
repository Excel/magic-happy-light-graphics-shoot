#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(GLuint textureId, float3 color, float3 velocity,
                                 float3 force, float scale, float fuzziness, float speed,
                                 unsigned maxParticles) :
                    m_maxParticles(maxParticles), m_textureID(textureId), m_speed(speed),
                    m_fuzziness(fuzziness), m_scale(scale), m_color(color), m_velocity(velocity),
                    m_force(force)
{
    m_particles = new Particle[maxParticles];
    resetParticles();

    for (unsigned i = 0; i < m_maxParticles; ++i)
        m_particles[i].active = false;
}

ParticleEmitter::~ParticleEmitter()
{
    if (m_particles)
    {

        glDeleteTextures(1, &m_textureID);
        delete[] m_particles;
        m_particles = 0;
    }
}

/**
  * You need to fill this in.
  *
  * Resets the particle at the given index to its initial state. Should reset the
  * position, direction, force, color, life, and decay of the particle.
  */
void ParticleEmitter::resetParticle(unsigned i)
{
    m_particles[i].pos.zero();

    m_particles[i].life = 1.0;
    m_particles[i].decay = urand(0.0025f, 0.15f);
    m_particles[i].color = m_color;
    m_particles[i].force.x = urand(-m_fuzziness*0.01f, m_fuzziness*0.01f+m_force.x);
    m_particles[i].force.y = urand(-m_fuzziness*0.01f, m_fuzziness*0.01f+m_force.y);
    m_particles[i].force.z = urand(-m_fuzziness*0.01f, m_fuzziness*0.01f+m_force.z);

    m_particles[i].dir.x = urand(-m_fuzziness, m_fuzziness+m_velocity.x);
    m_particles[i].dir.y = urand(-m_fuzziness, m_fuzziness+m_velocity.y);
    m_particles[i].dir.z = urand(-m_fuzziness, m_fuzziness+m_velocity.z);

    //Continue filling out code here
}

/**
  * Resets all particles in this emitter to their initial states
  */
void ParticleEmitter::resetParticles()
{
    for (unsigned i = 0; i < m_maxParticles; i++)
        resetParticle(i);
}

/**
  * You need to fill this in.
  *
  * Performs one step of the particle simulation. Should perform all physics
  * calculations and maintain the life property of each particle.
  */
void ParticleEmitter::updateParticles()
{
    for(unsigned i = 0; i < m_maxParticles; ++i)
    {
        if(!m_particles[i].active){
            m_particles[i].active = true;
            resetParticle(i);
        }
        else{
            m_particles[i].pos.x = m_particles[i].pos.x + m_particles[i].dir.x * m_speed;
            m_particles[i].pos.y = m_particles[i].pos.y + m_particles[i].dir.y * m_speed;
            m_particles[i].pos.z = m_particles[i].pos.z + m_particles[i].dir.z * m_speed;

            m_particles[i].dir.x += m_particles[i].force.x;
            m_particles[i].dir.y += m_particles[i].force.y;
            m_particles[i].dir.z += m_particles[i].force.z;

            m_particles[i].life -= m_particles[i].decay;
            if(m_particles[i].life < 0){
                m_particles[i].active = false;
            }
        }
    }
}

/**
  * You need to fill this in.
  *
  * Draws each particle as a small, texture-mapped square of side-length m_scale.
  * Each square should be in the X/Y plane at Z = the particle's position's Z-coordinate.
  */
void ParticleEmitter::drawParticles()
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glDepthMask(false);
    glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE);
     glBegin(GL_QUADS);
    for(unsigned i = 0; i < m_maxParticles; ++i){
        if(m_particles[i].active){

            glTexCoord2f(0, 0);
            glVertex3f(m_particles[i].pos.x, m_particles[i].pos.y, m_particles[i].pos.z);
            glTexCoord2f(0, 1.f);
            glVertex3f(m_particles[i].pos.x, m_particles[i].pos.y+m_scale, m_particles[i].pos.z);
            glTexCoord2f(1.f, 1.f);
            glVertex3f(m_particles[i].pos.x+m_scale, m_particles[i].pos.y+m_scale, m_particles[i].pos.z);
            glTexCoord2f(1.f, 0);
            glVertex3f(m_particles[i].pos.x+m_scale, m_particles[i].pos.y, m_particles[i].pos.z);




        }
    }
    glEnd();
    glDisable(GL_BLEND);

    glDepthMask(true);

    //Put your code here
}
