#include "Application.h"
#include "GameObject.h"
#include "Quadtree.h"


Quadtree::Quadtree()
{
	QT_Box = new AABB();
	QT_Box->SetNegativeInfinity();

	maxElements = 1;
	maxLevels = 3;
	level = 0;

	for (int i = 0; i < 4; i++)
	{
		children[i] = nullptr;
	}
}

Quadtree::Quadtree(AABB limits,int level)
{
	QT_Box = new AABB(limits);
	maxElements = 1;
	maxLevels = 3;
	this->level = level;

	for (int i = 0; i < 4; i++)
	{
		children[i] = nullptr;
	}
}


Quadtree::~Quadtree()
{
}

void Quadtree::Insert(GameObject * gameObject)
{
	if (gameObject != nullptr && QT_Box->Intersects(gameObject->boundingBox))
	{
		if (children[0]!=nullptr)
		{
			for (int i = 0; i < 4; i++)
			{
				children[i]->Insert(gameObject);
			}
		}
		else
		{
			container.push_back(gameObject);
			
			if (level<maxLevels && container.size() > maxElements)
			{
				if (children[0] == nullptr)
				{
					Split();
				}

				for (int j = 0; j < container.size(); j++)
				{
					for (int i = 0; i < 4; i++)
					{
						children[i]->Insert(container[j]);
					}
				}

				container.clear();
			}
		}
	}

}

void Quadtree::Split()
{
	/*view from top
 max ________ ________
	|        |        |
	|        |        |
	| first  | second |
	|        |        |
	 -------- --------
	|        |        |
	|        |        |
	| third  | fourth |
	|        |        |
	 -------- --------min*/

	float x, y, z;

	x = QT_Box->MaxX() - QT_Box->MinX();
	y = QT_Box->MaxY() - QT_Box->MinY();
	z = QT_Box->MaxZ() - QT_Box->MinZ();

	vec diagonal = { x / 2,y,z / 2 };

	//first Child
	AABB first({ QT_Box->minPoint }, { QT_Box->minPoint + diagonal });
	children[0]=new Quadtree(first,level+1);

	//second Child
	vec secondMin = QT_Box->minPoint + vec(x / 2, 0.0f, 0.0f);
	AABB second( secondMin ,  secondMin + diagonal );
	children[1] = new Quadtree(second, level + 1);

	//third Child
	vec thirdMin = QT_Box->minPoint + vec(0.0f, 0.0f, z/2);
	AABB third(thirdMin, thirdMin + diagonal);
	children[2] = new Quadtree(third, level + 1);

	//fourth Child
	AABB fourth(QT_Box->maxPoint-diagonal, QT_Box->maxPoint);
	children[3] = new Quadtree(fourth, level + 1);

}

void Quadtree::Remove(GameObject * gameObject)
{
}

void Quadtree::Intersect(vector<GameObject*>& gameObjects, const AABB & boundingBox)
{
	if (QT_Box->Intersects(boundingBox))
	{
		if (children[0] == nullptr)
		{
			for (int i = 0; i < container.size(); i++)
			{
				gameObjects.push_back(container[i]);
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				children[i]->Intersect(gameObjects, boundingBox);
			}
		}
	}
}

void Quadtree::Intersect(vector<GameObject*>& gameObjects, const Frustum& frustum)
{
	Plane planes[6];
	float3 BBcorners[8];
	int counter = 0;

	QT_Box->GetCornerPoints(BBcorners);
	frustum.GetPlanes(planes);

	for (int i = 0; i < 6; i++)
	{
		//This number checks if the bb is outside of a plane
		int aux_count = counter;

		for (int j = 0; j < 8; j++)
		{
			if (!planes[i].IsOnPositiveSide(BBcorners[j]))
			{
				counter++;
				break;
			}
		}
		if (aux_count == counter)
		{
			break;
		}
	}

	if (counter == 6)
	{
		if (children[0] == nullptr)
		{
			for (int i = 0; i < container.size(); i++)
			{
				gameObjects.push_back(container[i]);
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				children[i]->Intersect(gameObjects, frustum);
			}
		}
	}
}

void Quadtree::Clear()
{
	RELEASE(QT_Box);

	if (children[0] != nullptr)
	{
		for (int i = 0; i < 4; i++)
		{
			children[i]->Clear();
			RELEASE(children[i]);
		}
	}

	for (int i = 0; i < container.size(); i++)
	{
		container[i] = nullptr;
	}
	container.clear();

}

void Quadtree::DrawQT()
{
	if (children[0] != nullptr)
	{
		for (int i = 0; i < 4; i++)
		{
			children[i]->DrawQT();
		}
	}
	
	glLineWidth(2.5f);
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);

	glVertex3f(QT_Box->CornerPoint(0).x, QT_Box->CornerPoint(0).y, QT_Box->CornerPoint(0).z);
	glVertex3f(QT_Box->CornerPoint(1).x, QT_Box->CornerPoint(1).y, QT_Box->CornerPoint(1).z);

	glVertex3f(QT_Box->CornerPoint(0).x, QT_Box->CornerPoint(0).y, QT_Box->CornerPoint(0).z);
	glVertex3f(QT_Box->CornerPoint(2).x, QT_Box->CornerPoint(2).y, QT_Box->CornerPoint(2).z);

	glVertex3f(QT_Box->CornerPoint(0).x, QT_Box->CornerPoint(0).y, QT_Box->CornerPoint(0).z);
	glVertex3f(QT_Box->CornerPoint(4).x, QT_Box->CornerPoint(4).y, QT_Box->CornerPoint(4).z);

	glVertex3f(QT_Box->CornerPoint(3).x, QT_Box->CornerPoint(3).y, QT_Box->CornerPoint(3).z);
	glVertex3f(QT_Box->CornerPoint(1).x, QT_Box->CornerPoint(1).y, QT_Box->CornerPoint(1).z);

	glVertex3f(QT_Box->CornerPoint(3).x, QT_Box->CornerPoint(3).y, QT_Box->CornerPoint(3).z);
	glVertex3f(QT_Box->CornerPoint(2).x, QT_Box->CornerPoint(2).y, QT_Box->CornerPoint(2).z);

	glVertex3f(QT_Box->CornerPoint(3).x, QT_Box->CornerPoint(3).y, QT_Box->CornerPoint(3).z);
	glVertex3f(QT_Box->CornerPoint(7).x, QT_Box->CornerPoint(7).y, QT_Box->CornerPoint(7).z);

	glVertex3f(QT_Box->CornerPoint(6).x, QT_Box->CornerPoint(6).y, QT_Box->CornerPoint(6).z);
	glVertex3f(QT_Box->CornerPoint(2).x, QT_Box->CornerPoint(2).y, QT_Box->CornerPoint(2).z);

	glVertex3f(QT_Box->CornerPoint(6).x, QT_Box->CornerPoint(6).y, QT_Box->CornerPoint(6).z);
	glVertex3f(QT_Box->CornerPoint(4).x, QT_Box->CornerPoint(4).y, QT_Box->CornerPoint(4).z);

	glVertex3f(QT_Box->CornerPoint(6).x, QT_Box->CornerPoint(6).y, QT_Box->CornerPoint(6).z);
	glVertex3f(QT_Box->CornerPoint(7).x, QT_Box->CornerPoint(7).y, QT_Box->CornerPoint(7).z);

	glVertex3f(QT_Box->CornerPoint(5).x, QT_Box->CornerPoint(5).y, QT_Box->CornerPoint(5).z);
	glVertex3f(QT_Box->CornerPoint(1).x, QT_Box->CornerPoint(1).y, QT_Box->CornerPoint(1).z);

	glVertex3f(QT_Box->CornerPoint(5).x, QT_Box->CornerPoint(5).y, QT_Box->CornerPoint(5).z);
	glVertex3f(QT_Box->CornerPoint(4).x, QT_Box->CornerPoint(4).y, QT_Box->CornerPoint(4).z);

	glVertex3f(QT_Box->CornerPoint(5).x, QT_Box->CornerPoint(5).y, QT_Box->CornerPoint(5).z);
	glVertex3f(QT_Box->CornerPoint(7).x, QT_Box->CornerPoint(7).y, QT_Box->CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);
}

