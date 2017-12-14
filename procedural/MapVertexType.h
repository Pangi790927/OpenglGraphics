#ifndef MAP_VERTEX_TYPE_H
#define MAP_VERTEX_TYPE_H

#include "Mesh.h"

typedef Vertex <
	VertexAttribute <
		int, 		VertexTexID				, VertexAttribute <
		Point2f, 	VertexTexCoord			, VertexAttribute <
		Point3f, 	VertexNormal			, VertexAttribute <
		Point3f, 	VertexPosition			, VertexAttribute <
		Point4f, 	VertexColor				, VertexAttributeEnd	
> > > > > >
MapVertexType;

#endif