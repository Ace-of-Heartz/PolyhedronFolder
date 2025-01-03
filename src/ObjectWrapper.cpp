//
// Created by ace on 2025.01.03..
//

#include "ObjectWrapper.h"

ObjectWrapper::~ObjectWrapper()
{

}


template <typename VertexT>
void ObjectWrapper::CreateObject(MeshObject<VertexT>& mesh, std::initializer_list<VertexAttributeDescriptor>& attributes)
{
    object = CreateMeshObject(mesh, attributes);
}
