#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_

#include <algorithm> //std::copy

template <typename T,  bool accept_data_when_full = true> class CircularBuffer{
public:
	CircularBuffer(T* buffer, unsigned long int size, unsigned long int contentsize=0):
		_buffer(buffer),
		_buffersize(size),
		_head(contentsize%size),
		_tail(0),
		_contentsize(contentsize),
		_memoryallocated(false)
	{
		if(!_buffer){
			_buffer = new T[size];
			if(_buffer == nullptr){
				_memoryallocated = false;
			}
			_memoryallocated = true;
		}
	}
	~CircularBuffer(){
		if(_memoryallocated){
			delete[] _buffer;
		}
	}

	void resize(unsigned long int size){
		if(_buffer){
			delete(_buffer);
		}
		_buffersize = size;
		_tail = 0;
		_contentsize = 0;
		_buffer = new T[size];
		if(_buffer == nullptr){
			_memoryallocated = false;
			return;
		}
		_memoryallocated = true;
		return;
	}


	unsigned long int In(const T &item){
		return In(&item, 1);
	}

	unsigned long int In(const T* buffer, unsigned long int size){
		if(size==0){
			return size;
		}
		unsigned long int temp=Free();
		if(size>temp){
			if(accept_data_when_full){
				Out(0, size-temp);
				temp=Free();
			}
			size=temp;
		}
		temp=0;
		if((_head+size)>=_buffersize){
			temp=(_buffersize-_head);
			std::copy(buffer, buffer+temp, _buffer+_head);
			_head=0;
		}
		std::copy(buffer+temp, buffer+size, _buffer+_head);
		_contentsize+=size;
		_head+=size-temp;
		return size;
	}
	unsigned long int InBeginning(const T &item){
		return InBeginning(&item, 1);
	}

	unsigned long int InBeginning(const T* buffer, unsigned long int size){
		if(size==0){
			return size;
		}
		unsigned long int temp=Free();
		if(size>temp){
			if(accept_data_when_full){
				Out(0, size-temp);
				temp=Free();
			}
			size=temp;
		}
		temp=0;

		if(_tail < size){
			temp=_tail;
			std::copy(buffer, buffer+temp, _buffer);
			_tail=_buffersize;
		}
		std::copy(buffer+temp, buffer+size, _buffer+_tail-size);
		_contentsize+=size;
		_tail-=(size-temp);
		_tail%=_buffersize;
		return size;
	}

	unsigned long int Out(CircularBuffer<T> &CB, unsigned long int size){
		if(size==0){
			return size;
		}
		T cb[size];
		size=Out(cb, size);
		CB.In(cb, size);
		return size;
	}

	unsigned long int Out(T* buffer, unsigned long int size){
		if(size==0){
			return size;
		}
		unsigned long int temp=Ocupied();
		if(size>temp){
			size=temp;
		}
		if(!size) return 0;
		temp=0;
		if((_tail+size)>=_buffersize){
			temp=(_buffersize-_tail);
			if(buffer) std::copy(_buffer+_tail, _buffer+_tail+temp, buffer);
			_tail=0;
		}
		if(buffer) std::copy(_buffer+_tail, _buffer+_tail+size-temp, buffer+temp);
		_contentsize-=size;
		_tail+=size-temp;
		if(!_contentsize){
			Clear();
		}
		return size;
	}

	T Out(){
		T temp;
		Out(&temp,1);
		return temp;
	}

	unsigned long int OutEnd(T* buffer, unsigned long int size){
		if(size==0){
			return size;
		}
		unsigned long int temp=Ocupied();
		if(size>temp){
			size=temp;
		}
		if(!size) return 0;
		temp=size;
		if(_head<size){
			temp=(size-_head);
			if(buffer) std::copy(_buffer, _buffer+_head, buffer+temp);
			_head=0;
		}
		if(_head==0){
			_head=_buffersize;
		}
		if(buffer) std::copy(_buffer+_head-temp, _buffer+_head, buffer);
		_contentsize-=size;
		_head-=temp;
		_head%=_buffersize;
		if(!_contentsize){
			Clear();
		}
		return size;
	}

	T &operator[](unsigned long int index){
		return _buffer[(_tail+index)%_buffersize];
	}

	unsigned long int Free(){
		return _buffersize-_contentsize;
	}
	unsigned long int Ocupied(){
		return _contentsize;
	}
	void Clear(){
		_head=0;
		_tail=0;
		_contentsize=0;
	}

	T* GetRearrangedBuffer(){
		if(_tail!=0){
			unsigned long int size=Ocupied();
			T temp[size];
			Out(temp, size);
			In(temp, size);
		}
		//TODO Rearrange Buffer
		return _buffer;
	}

protected:
	T* _buffer;
	unsigned long int _buffersize;
	unsigned long int _head;
	unsigned long int _tail;
	unsigned long int _contentsize;
	bool _memoryallocated;
};

#endif /* INC_CIRCULARBUFFER_H_ */
