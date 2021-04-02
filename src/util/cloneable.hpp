#pragma once

#include <memory>

/**
 * Utility for creating shared_ptr clones of objects.
 */
template<typename T>
class Cloneable {
    public:
        /**
         * Clones this object as type T.
         */
        std::shared_ptr<T> clone() {
            return std::shared_ptr<T>(this->clone_implementation());
        }
    
    protected:
        /**
         * Implementation of clone using raw pointers.
         */
        virtual T* clone_implementation() = 0;
};

/**
 * Utility for creating 2nd layer clone.
 */
template<typename T>
class Cloneable2 {
    public:
        /**
         * Clones this object as type T.
         */
        std::shared_ptr<T> clone2() {
            return std::shared_ptr<T>(this->clone_implementation());
        }

    protected:
        /**
         * Implementation of clone using raw pointers.
         */
        virtual T* clone_implementation() = 0;
};