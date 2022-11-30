#include <Object/physicsState.h>

void PhysicsState::updateWorldMesh(){
    mesh.updateWorldMesh(x,y,theta,scale);
}