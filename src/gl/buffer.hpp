#pragma once

#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../util/delayed_init.hpp"

/**
 * Generic OpenGL buffer.
 */
template <typename T>
class Buffer : public DelayedInit {
    public:
        /**
         * Shared_ptr constructor of Buffer.
         */
        static std::shared_ptr<Buffer<T>> makeBuffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) {
            std::shared_ptr<Buffer<T>> buffer(new Buffer<T>(vectors, type, index, size));

            return buffer;
        }

        /**
         * Generates the OpenGL buffer (this is delayed to run when back on parent thread).
         */
        virtual void delayedInit() override {
            DelayedInit::delayedInit();

            GLuint buffer;

            glGenBuffers(1, &buffer);
            glBindBuffer(type, buffer);
            glBufferData(type, this->vectors.size() * sizeof(T), &vectors[0], GL_STATIC_DRAW);

            if(this->index >= 0) {
                glVertexAttribPointer(
                    this->index, 
                    this->size, 
                    GL_FLOAT, 
                    GL_FALSE, 
                    0, 
                    0
                );

                glEnableVertexAttribArray(index);
            }
        }
    
    protected:
        virtual Buffer* cloneImplementation() override {
            return new Buffer(*this);
        }

    private:
        /**
         * The OpenGL buffer type.
         */
        GLenum type;
        /**
         * The vertex attrib pointer index (this is only really used for GL_ARRAY_BUFFER).
         */
        int index;
        /**
         * The buffer type size.
         */
        int size;
        /**
         * The raw vector of points for the buffer.
         */
        std::vector<T> vectors;

        Buffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) : 
            vectors(vectors), 
            type(type), 
            index(index), 
            size(size) 
        {}

        Buffer(const Buffer& buffer) :
            DelayedInit(buffer),
            type(buffer.type),
            index(buffer.index),
            size(buffer.size)
        {
            for(auto e : buffer.vectors) {
                this->vectors.push_back(e);
            }
        }
};

namespace pepng {
    template <typename T>
    std::shared_ptr<Buffer<T>> makeBuffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) {
        return Buffer<T>::makeBuffer(vectors, type, index, size);
    }
}