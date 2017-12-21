#ifndef ALLOC_ON_VEC_H
#define ALLOC_ON_VEC_H

template <typename Type>
class AllocOnVec {
public:
	static std::vector<Type> emptyType;

	std::reference_wrapper<std::vector<Type>> elems;
	std::stack<int> emptyElems;

	int start;
	int size;
	int top = 0;

	int totalElems = 0;

	AllocOnVec () : elems(emptyType) {}

	AllocOnVec (int start, int size, std::vector<Type> &elems) 
			: elems(elems), size(size), start(start) {}

	int count() {
		return top;
	}

	int isEmpty() {
		return count() <= 0;
	}

	int isFull() {
		return count() >= size;
	}

	int addElem() {
		totalElems++;
		// std::cout << "totalElems: " << totalElems << " " << size << std::endl;

		if (!emptyElems.empty()) {
			int index = emptyElems.top();
			emptyElems.pop();
			return localPosToGlobal(index);
		}

		if (!isFull()) {
			return localPosToGlobal(top++);
		}
		DEBUG("ERROR: OUT OF MEMORY ... Total Elems:" << totalElems << " size: " << size);
		std::exit(-12);
		return -1;
	}

	void popFrom (int globalPos) {
		totalElems--;
		emptyElems.push(globalPosToLocal(globalPos));
	}

	Type &get (int globalPos) {
		return elems.get()[globalPos];
	}

	int localPosToGlobal (int localPos) {
		return localPos + start;
	}

	int globalPosToLocal (int globalPos) {
		return globalPos - start;
	}
};

template <typename Type>
std::vector<Type> AllocOnVec<Type>::emptyType;

#endif