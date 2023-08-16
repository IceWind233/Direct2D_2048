#ifndef COMPTR_H
#define COMPTR_H

#include <memory>

template <typename com_t>
class com_ptr {
public:

	com_ptr() noexcept = default;

	com_ptr(nullptr_t);

	com_ptr(com_t _com);

	~com_ptr();

public:

	operator bool() const noexcept;

	com_t& operator*() const noexcept;

	com_t* operator->() const noexcept;

public:

	void reset();

	com_t* get() const noexcept;

	com_t** set() noexcept;

private:

	void safe_release();

private:

	com_t* ptr_;
};

template <typename com_t>
com_ptr<com_t>::com_ptr(nullptr_t)
	: com_ptr() {}

template <typename com_t>
com_ptr<com_t>::com_ptr(com_t _com) :
	ptr_(_com){}

template <typename com_t>
com_ptr<com_t>::~com_ptr() {
	safe_release();
}

template <typename com_t>
com_ptr<com_t>::operator bool() const noexcept {
	return static_cast<bool>(ptr_);
}

template <typename com_t>
com_t& com_ptr<com_t>::operator*() const noexcept {
	return *ptr_;
}

template <typename com_t>
com_t* com_ptr<com_t>::operator->() const noexcept {
	return ptr_;
}

template <typename com_t>
void com_ptr<com_t>::reset() {
	safe_release();
}

template <typename com_t>
com_t* com_ptr<com_t>::get() const noexcept {
	return ptr_;
}

template <typename com_t>
com_t** com_ptr<com_t>::set() noexcept {
	if (ptr_) {
		return &ptr_;
	}
}

template <typename com_t>
void com_ptr<com_t>::safe_release() {
	if(ptr_ != nullptr) {
		std::exchange(ptr_, {})->Release();
	}
}

#endif