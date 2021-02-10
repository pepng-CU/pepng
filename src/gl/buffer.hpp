#pragma once

#include <memory>
#include <GL/glew.h>
#include "../util/delayed_init.hpp"

template <typename T>
class Buffer : public DelayedInit {
    public:
        static std::shared_ptr<Buffer<T>> makeBuffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) {
            std::shared_ptr<Buffer<T>> buffer(new Buffer<T>(vectors, type, index, size));

            return buffer;
        }

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

    private:
        Buffer(std::vector<T> vectors, GLenum type, int index = -1, int size = -1) : vectors(vectors), type(type), index(index), size(size) {}
        
        GLenum type;
        int index;
        int size;
        std::vector<T> vectors;
};