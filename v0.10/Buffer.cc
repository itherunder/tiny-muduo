#include "Buffer.h"

Buffer::Buffer() {
    // cout << "[CONS] Buffer ..." << endl;
}

Buffer::~Buffer() {
    // cout << "[DECO] ~Buffer ..." << endl;
}

const char* Buffer::Peek() {
    return buffer_.c_str();
}

void Buffer::Append(const string& str) {
    buffer_.append(str);
}

void Buffer::Retrieve(int len) {
    buffer_ = buffer_.substr(len);
}

string Buffer::RetrieveAllAsString() {
    return RetrieveAsString(ReadableBytes());
}

string Buffer::RetrieveAsString(int len) {
    string res(Peek(), len);
    Retrieve(len);
    return res;
}

int Buffer::ReadableBytes() {
    return buffer_.length();
}
