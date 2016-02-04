//
//  ringbuffer.h
//  csvtools
//
//  Created by qna on 01/06/15.
//  Copyright (c) 2014 qna. All rights reserved.
//

#ifndef __csvtools__ringbuffer__
#define __csvtools__ringbuffer__

#include <vector>


template<class T>
class ringbuffer {
private:
    size_t m_size;
    size_t m_length;
    size_t m_tail;
    size_t m_front;
    std::vector<T> m_buf;
public:
    ringbuffer() : m_size(100), m_length(0), m_tail(0), m_front(0), m_buf(100) { }
    ringbuffer(size_t bufsize) : m_size(bufsize), m_length(0), m_tail(0), m_front(0), m_buf(bufsize) { }
    
    T &operator[](size_t pos) {
        if (pos >= m_length) {
            throw new std::exception();
        }
        return m_buf[(m_front+pos)%m_size];
    }
    
    void push_back(T& t) {
        m_buf[m_tail] = t;
        m_tail = (m_tail + 1) % m_size;
        m_length++;
        if (m_length > m_size) {
            m_length = m_size;
            m_front = (m_front + 1) % m_size;
        }
    }
    
    void clear() {
        m_length = m_tail = m_front = 0;
    }
    
    const size_t length() { return m_length; }
    const size_t size() { return m_size; }
};

#endif