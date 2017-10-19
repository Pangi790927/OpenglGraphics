#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include "MTLLoader.h"

template <typename compFirst, typename compSecond, typename firstType, typename secondType>
class TypeChose{
public: 
	typedef secondType type; 
};

template <typename compFirst, typename firstType, typename secondType>
class TypeChose <compFirst, compFirst, firstType, secondType> {
public: 
	typedef firstType type; 
};

template <typename compFirst, typename compSecond>
class TypeEqual{
public: 
	static constexpr bool areEqual() {
		return false; 
	}
};

template <typename compFirst>
class TypeEqual<compFirst, compFirst>{
public: 
	static constexpr bool areEqual() {
		return true; 
	}
};

template <class compFirst, class compSecond, typename firstType, typename secondType>
class ReferenceChoser {
public:
	static secondType& referenceChose (firstType& arg1, secondType& arg2) {
		return arg2; 
	}
};

template <class compFirst, typename firstType, typename secondType>
class ReferenceChoser <compFirst, compFirst, firstType, secondType> {
public:
	static firstType& referenceChose (firstType& arg1, secondType& arg2) {
		return arg1; 
	}
};

template <int number, typename firstType, typename secondType>
class NumberChose {
public: 
	typedef secondType type; 
};

template <typename firstType, typename secondType>
class NumberChose <0, firstType, secondType> {
public: 
	typedef firstType type; 
};

template <int number, typename firstType, typename secondType>
class ReferenceChoserByNumber {
public:
	static secondType& referenceChose (firstType& arg1, secondType& arg2) {
		return arg2; 
	}
};

template <typename firstType, typename secondType>
class ReferenceChoserByNumber <0, firstType, secondType> {
public:
	static firstType& referenceChose (firstType& arg1, secondType& arg2) {
		return arg1; 
	}
};

template <typename Type, class TypeDescriptor, typename VertexAttr>
class VertexAttribute : VertexAttr {
public:
	Type attribute;
	
	typedef TypeDescriptor indexingType;
	typedef VertexAttr nextAttribute;

	static constexpr int getTypeCount() {
		return 1 + VertexAttr::getTypeCount();
	}

	static constexpr int getTypeSize (int typeID) {
		return typeID == 0 ? sizeof(Type) : VertexAttr::getTypeSize(typeID - 1);
	}

	template <class CompareTypeDescriptor>
	class GetType {
	public: 
		typedef typename TypeChose<CompareTypeDescriptor, TypeDescriptor, 
			Type, typename VertexAttr::template GetType<CompareTypeDescriptor>::type>::type type;
	};

	template <class CompareTypeDescriptor>
	typename GetType<CompareTypeDescriptor>::type& GetVariable() {
		return ReferenceChoser<CompareTypeDescriptor, TypeDescriptor, 
						Type, 
						typename VertexAttr::template GetType<CompareTypeDescriptor>::type>::referenceChose
					(
						attribute, 
						VertexAttr::template GetVariable<CompareTypeDescriptor>()
					);
	}

	template <int number>
	class GetTypeByNumber {
	public: 
		typedef typename NumberChose<number, 
			Type, typename VertexAttr::template GetTypeByNumber<number - 1>::type>::type type;
	};

	template <int number>
	typename GetTypeByNumber<number>::type& GetVariableByNumber() {
		return ReferenceChoserByNumber<number, 
						Type, 
						typename VertexAttr::template GetTypeByNumber<number - 1>::type>::referenceChose
					(
						attribute, 
						VertexAttr::template GetVariableByNumber<number - 1>()
					);
	}
};

class VertexAttributeEnd {	
public:
}; 

class EmptyType {
public:
	static EmptyType emptyVar; 
};
EmptyType EmptyType::emptyVar;

template <typename Type, class TypeDescriptor>
class VertexAttribute <Type, TypeDescriptor, VertexAttributeEnd> : VertexAttributeEnd {
public:
	Type attribute;
	
	typedef TypeDescriptor indexingType;
	typedef VertexAttributeEnd nextAttribute;

	// static EmptyType var; 

	static constexpr int getTypeCount() {
		return 1;
	}

	static constexpr int getTypeSize (int typeID) {
		return typeID == 0 ? sizeof(Type) : getTypeSize(typeID - 1);
	}

	template <class CompareTypeDescriptor>
	class GetType {
	public: 
		typedef typename TypeChose<CompareTypeDescriptor, TypeDescriptor, 
			Type, EmptyType>::type type;
	};

	template <class CompareTypeDescriptor>
	typename GetType<CompareTypeDescriptor>::type& GetVariable() {
		return ReferenceChoser<CompareTypeDescriptor, TypeDescriptor, 
					Type, 
					EmptyType>::referenceChose
				(
					attribute, EmptyType::emptyVar
				);
	}

	template <int number>
	class GetTypeByNumber {
	public: 
		typedef typename NumberChose<number, 
			Type, EmptyType>::type type;
	};

	template <int number>
	typename GetTypeByNumber<number>::type& GetVariableByNumber() {
		return ReferenceChoserByNumber<number,	
					Type, 
					EmptyType>::referenceChose
				(
					attribute, EmptyType::emptyVar
				);
	}
};

template <typename AttrType, int number>
class _GetNodeTypeByNumber {
public:
	typedef typename _GetNodeTypeByNumber<typename AttrType::nextAttribute, number - 1>::type type; 
};

template <typename AttrType>
class _GetNodeTypeByNumber<AttrType, 0> {
public:
	typedef AttrType type; 
};

// template <typename Type, class TypeDescriptor>
// typename VertexAttribute <Type, TypeDescriptor, VertexAttributeEnd>::EmptyType 
// VertexAttribute <Type, TypeDescriptor, VertexAttributeEnd>::var; 

class VertexNone 		{ public: constexpr static const char* name = "VertexNone"; };	
class VertexPosition 	{ public: constexpr static const char* name = "VertexPosition"; };
class VertexNormal 		{ public: constexpr static const char* name = "VertexNormal"; };
class VertexTexCoord 	{ public: constexpr static const char* name = "VertexTexCoord"; };
class VertexTexID 		{ public: constexpr static const char* name = "VertexTexID"; };
class VertexColor		{ public: constexpr static const char* name = "VertexColor"; };

template <template <typename A, typename B, typename C, typename D> class FunctionToMap, typename MapArguments
		, int iteration = 0, typename vertexType>
typename std::template enable_if <iteration == vertexType::getTypeCount(), void>::type 
mapFunction (vertexType&, MapArguments&) {}

template <
	template <typename A, typename B, typename C, typename D> class FunctionToMap, 
	typename MapArguments,
	int iteration = 0, 
	typename vertexType
>
typename std::template enable_if <iteration < vertexType::getTypeCount(), void>::type 
mapFunction (vertexType& vert, MapArguments& mapArgs) {
	
	FunctionToMap <
		typename vertexType::template GetAttrTypeByNumber<iteration>::type, 
		vertexType, 
		MapArguments, 
		typename vertexType::template GetNodeTypeByNumber<iteration>::type 
	>
	() (vert.template getByNumber<iteration>(), vert, mapArgs);

	// std::cout << vert.get<iteration>() << std::endl;
	mapFunction <FunctionToMap, MapArguments, iteration + 1, vertexType> (vert, mapArgs); 
}

template <typename AttrType, typename VertexType, typename StreamType, typename NodeType>
class OstreamFromVertex {
public:
	void operator () (AttrType& attr, VertexType& vert, StreamType& stream) {
		stream << NodeType::indexingType::name << " -> " << attr << ", "; 
	}
};

template <typename AttributeType>
class Vertex {
public:
 	AttributeType attribute; 

 	Vertex () = default;

 	Vertex (const Vertex& vertex) : attribute(vertex.attribute) {}

 	Vertex (Vertex& vertex) : Vertex(const_cast<const Vertex&>(vertex)) {}

 	template <typename... Args>
 	Vertex (Args&&... args) {
 		initAttribute<0>(args...);
 	}

 	template <int number, typename ArgType, typename... Args>
 	void initAttribute (ArgType&& arg, Args&&... args) {
 		getByNumber<number> () = arg; 

 		initAttribute<number + 1>(args...); 
 	}

 	template <int number, typename ArgType>
 	void initAttribute (ArgType&& arg) {
 		getByNumber<number> () = arg; 
	}

 	static constexpr int getTypeCount() {
		return AttributeType::getTypeCount();
	}

	static constexpr int getTypeSize (int typeID) {
		return AttributeType::getTypeSize(typeID);
	}

	template <class TypeDescriptor>
	class GetAttrType {
	public: 
		typedef typename AttributeType::template GetType<TypeDescriptor>::type type;
	};

	template <class TypeDescriptor>
	typename GetAttrType<TypeDescriptor>::type &get() {
		return attribute.template GetVariable<TypeDescriptor>(); 
	}

	template <int number>
	class GetAttrTypeByNumber {
	public: 
		typedef typename AttributeType::template GetTypeByNumber<number>::type type;
	};

	template <int number>
	typename GetAttrTypeByNumber<number>::type &getByNumber() {
		return attribute.template GetVariableByNumber<number>(); 
	}

	template <int number>
	class GetNodeTypeByNumber {
	public:
		typedef typename _GetNodeTypeByNumber<AttributeType, number>::type type; 	
	};

	template <typename TypeDescriptor>
	constexpr static bool hasVertexAttribute() {
		return !(TypeEqual <typename GetAttrType<TypeDescriptor>::type, EmptyType>::areEqual()); 
	}

	template <typename TypeDescriptor, typename AttrType>
	typename std::enable_if<hasVertexAttribute<TypeDescriptor>(), void>::type 
	setIfExists (AttrType& attr) {
		get<TypeDescriptor>() = attr; 
	}

	template <typename TypeDescriptor, typename AttrType>
	typename std::enable_if<!hasVertexAttribute<TypeDescriptor>(), void>::type 
	setIfExists (AttrType& attr) {
		; 	/// do nothing
	}

	template <typename TypeDescriptor, typename Function>
	typename std::enable_if<hasVertexAttribute<TypeDescriptor>(), void>::type 
	useIfExists (Function&& func) {
		func(get<TypeDescriptor>()); 
	}

	template <typename TypeDescriptor, typename Function>
	typename std::enable_if<!hasVertexAttribute<TypeDescriptor>(), void>::type 
	useIfExists (Function&& func) {
		; 	/// do nothing
	}

	friend std::ostream& operator << (std::ostream& stream, Vertex& arg) {
		mapFunction <OstreamFromVertex, std::ostream> (arg, stream); 

		return stream; 
	}
}; 

typedef Vertex <
	VertexAttribute <
		long long, VertexTexID				, VertexAttribute <
		Point2i, VertexTexCoord				, VertexAttribute <
		Point4i, VertexNormal				, VertexAttribute <
		Point3i, VertexPosition				, VertexAttribute <
		long long, VertexTexID				, VertexAttributeEnd	
> > > > > >
VertexTypeDef1; 

// template <typename AttributeType, typename VertexType>
// class FunctionToMap {
// public:
// 	void operator () (AttributeType& attr, VertexType& vert) {
// 		cout << "Memory displacement :" << endl; 
// 		cout << (((char*)&attr) - ((char*)&vert)) << endl;
// 	}
// };

// VertexType::getTypeCount()
// VertexType::getTypeSize();
// simpleVertex.get<0>()
// simpleVertex.get<IdentifierClass>();
// mapFunction <FunctionToMap> (simpleVertex);

template <typename VertexType>
class Mesh {
public:
	std::vector <VertexType> vertexList; 
	std::vector <Material> materials;
	std::vector <int> materialIndex;
	std::vector <std::vector<int>> elementIndex;

	Mesh() {
		materialIndex.push_back(0);
	}

	Material getMaterialByIndex (int index) {
		if (index >= 0 && index < materials.size())
			return materials[index];
		else
			return Material();
	}

	int getVertCount() {
		return vertexList.size();
	}

	void addVertex (VertexType vertex) {
		vertexList.emplace_back(vertex);
	}

	friend std::ostream& operator << (std::ostream& stream, Mesh& arg) {
		for (auto&& vert : arg.vertexList)
			stream << vert << std::endl;

		for (auto&& polygon : arg.elementIndex) {
			for (auto&& index : polygon) {
				stream << index << " ";
			}
			stream << std::endl;
		}

		return stream;
	}
};


#endif
