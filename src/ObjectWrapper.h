//
// Created by ace on 2025.01.03..
//

#ifndef OBJECT_H
#define OBJECT_H
#include "GLUtils.hpp"
#include "Transform.h"


class ObjectWrapper {

public:
  ObjectWrapper() = default;
  explicit ObjectWrapper(const OGLObject& object) : object(object) {}

  virtual ~ObjectWrapper();

  [[nodiscard]] OGLObject& GetObject() {return object;}
  [[nodiscard]] Transform& GetLocalTransform() {return localTransform;};

  template <typename VertexT>
  void CreateObject (MeshObject<VertexT>& mesh, std::initializer_list<VertexAttributeDescriptor>& attributes);
  void SetObject(OGLObject object) {this->object = object;}
  void ClearObject() { this->object = OGLObject(); }
private:
  OGLObject object = {};
  Transform localTransform = {};
};



#endif //OBJECT_H
