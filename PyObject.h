#pragma once
#include<iostream>
class PyObject;
class BaseHolder {
public:
    virtual ~BaseHolder() {}
    virtual BaseHolder* getRef() = 0;
    virtual void assign(PyObject* p) = 0;
};
template<typename T>
class RefHolder: public BaseHolder {
public:
    T* ptr;
    RefHolder(T& val): ptr(&val) {}
    RefHolder<T>* getRef() {
        return new RefHolder<T>(*ptr);
    }
    void assign(PyObject* p);
};
template<typename T>
class ValHolder: public BaseHolder {
public:
    T val;
    template<typename U>
    ValHolder(U&& val): val(std::forward<U>(val)) {}
    RefHolder<T>* getRef() {
        return new RefHolder<T>(val);
    }
    void assign(PyObject* p) {
        std::cout << "Owning" << std::endl;
        (*p) = std::move(val);
    }
};

class PyObject {
private:
    BaseHolder* holder;
public:
    PyObject() {
        holder = nullptr;
    }
    template<typename T>
    PyObject& operator=(T&& val) {
        std::cout << "PyObject got a value" << std::endl;
        if(holder != nullptr) delete holder;
        if(std::is_fundamental<T>::value) holder = new ValHolder<T>(val);
        else {
            holder = new ValHolder<T>(std::forward<T>(val));
            std::cout << "Owning" << std::endl;
        }
        return *this;
    }
    template<typename T>
    PyObject& operator=(T& val) {
        std::cout << "PyObject got a value" << std::endl;
        if(holder != nullptr) delete holder;
        if(std::is_fundamental<T>::value) holder = new ValHolder<T>(val);
        else {
            holder = new RefHolder<T>(val);
            std::cout << "Borrowing" << std::endl;
        }
        return *this;
    }
    PyObject& operator=(PyObject& p) {
        std::cout << "PyObject got a value" << std::endl;
        if(holder != nullptr) delete holder;
        p.holder->assign(this);
        return *this;
    }
    PyObject& operator=(PyObject* p) {
        std::cout << "PyObject got a value" << std::endl << "Borrowing" << std::endl;
        if(holder != nullptr) delete holder;
        holder = (*p).holder->getRef();
        return *this;
    }
    template<typename T>
    operator T&() {
        ValHolder<typename std::remove_cv<T>::type>* vh = dynamic_cast<ValHolder<typename std::remove_cv<T>::type>*>(holder);
        if(vh != nullptr) return vh->val;
        RefHolder<typename std::remove_cv<T>::type>* rh = dynamic_cast<RefHolder<typename std::remove_cv<T>::type>*>(holder);
        if(rh != nullptr) return *(rh->ptr);
        throw std::bad_cast();
    }
    template<typename T>
    friend void ValHolder<T>::assign(PyObject* p);
    template<typename T>
    friend void RefHolder<T>::assign(PyObject* p);
};

template<typename T>
void RefHolder<T>::assign(PyObject* p) {
    std::cout << "Borrowing" << std::endl;
    (*p).holder = new RefHolder<T>(*ptr);
}