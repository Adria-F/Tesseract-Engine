#ifndef __QUADTREE_H__
#define __QUADTREE_H__

using namespace std;

class Quadtree
{
public:

	Quadtree(AABB limits);
	~Quadtree();

	void Insert(GameObject* gameObject);
	void Remove(GameObject* gameObject);

	void Intersect(vector<GameObject*>& gameObjects, const AABB& boundingBox);

	void Clear();


private:

	AABB QTBox;

	list<GameObject*> container;

	int maxElements = 0;

};

#endif // !__QUADTREE_H__

