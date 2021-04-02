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
        static std::shared_ptr<Buffer<T>> make_buffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) {
            std::shared_ptr<Buffer<T>> buffer(new Buffer<T>(vectors, type, index, size));

            return buffer;
        }

        /**
         * Generates the OpenGL buffer (this is delayed to run when back on parent thread).
         */
        virtual void delayed_init() override {
            DelayedInit::delayed_init();

            GLuint buffer;

            glGenBuffers(1, &buffer);
            glBindBuffer(this->__type, buffer);
            glBufferData(this->__type, this->__vectors.size() * sizeof(T), &this->__vectors[0], GL_STATIC_DRAW);

            if(this->__index >= 0) {
                glVertexAttribPointer(
                    this->__index, 
                    this->__size, 
                    GL_FLOAT, 
                    GL_FALSE, 
                    0, 
                    0
                );

                glEnableVertexAttribArray(this->__index);
            }
        }
    
    protected:
        virtual Buffer* clone_implementation() override {
            return new Buffer(*this);
        }

    private:
        /**
         * The OpenGL buffer type.
         */
        GLenum __type;
        /**
         * The vertex attrib pointer index (this is only really used for GL_ARRAY_BUFFER).
         */
        int __index;
        /**
         * The buffer type size.
         */
        int __size;
        /**
         * The raw vector of points for the buffer.
         */
        std::vector<T> __vectors;

        Buffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) : 
            __vectors(vectors), 
            __type(type), 
            __index(index), 
            __size(size) 
        {}

        Buffer(const Buffer& buffer) :
            DelayedInit(buffer),
            __type(buffer.__type),
            __index(buffer.__index),
            __size(buffer.__size)
        {
            for(auto e : buffer.__vectors) {
                this->__vectors.push_back(e);
            }
        }
};

namespace pepng {
    template <typename T>
    std::shared_ptr<Buffer<T>> make_buffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) {
        return Buffer<T>::make_buffer(vectors, type, index, size);
    }
}