#ifndef __QUADTREE_H__
#define __QUADTREE_H__

using namespace std;

class Quadtree
{
public:

	Quadtree();
	Quadtree(AABB limits);
	~Quadtree();

	void Insert(GameObject* gameObject);
	void Split();
	void Remove(GameObject* gameObject);

	void Intersect(vector<GameObject*>& gameObjects, const AABB& boundingBox);
	void DrawQT();

	void Clear();


private:

	AABB QT_Box;

	vector<GameObject*> container;

	int maxElements = 0;

	vector<Quadtree*> children;

};

#endif // !__QUADTREE_H__

