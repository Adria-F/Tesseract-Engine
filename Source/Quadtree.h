#ifndef __QUADTREE_H__
#define __QUADTREE_H__

using namespace std;

class Quadtree
{
public:

	Quadtree();
	Quadtree(AABB limits, int level);
	~Quadtree();

	void Insert(GameObject* gameObject);
	void Split();
	void Remove(GameObject* gameObject);

	void Intersect(vector<GameObject*>& gameObjects, const AABB& boundingBox);
	void Intersect(list<GameObject*>& gameObjects, const Frustum& boundingBox);
	void DrawQT();

	void Clear();


public:

	AABB* QT_Box;

private:

	vector<GameObject*> container;

	int maxElements = 0;
	int maxLevels = 0;
	int level = 0;

	Quadtree* children[4];

};

#endif // !__QUADTREE_H__

