#pragma once

class EngineException : public std::exception {};

class WindowError : public EngineException {
private:
    std::string message;
public:
    WindowError(const char* msg) : message(msg) {}

    const char* what() const noexcept {
        return message.c_str();
    }
};

class TextureError : public EngineException {
private:
    std::string message;
public:
    TextureError(const char* msg) : message(msg) {}

    const char* what() const noexcept {
        return message.c_str();
    }
};

class ObjectError : public EngineException {
private:
    std::string message;
public:
    ObjectError(const char* msg) : message(msg) {}

    const char* what() const noexcept {
        return message.c_str();
    }
};