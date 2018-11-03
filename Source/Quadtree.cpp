#include "Application.h"
#include "GameObject.h"
#include "Quadtree.h"


Quadtree::Quadtree()
{
	QT_Box.minPoint = { -10.0f,-10.0f,-10.0f };
	QT_Box.maxPoint = { 10.0f,10.0f,10.0f };

	maxElements = 1;
}

Quadtree::Quadtree(AABB limits)
{
	QT_Box = limits;
	maxElements = 1;
}


Quadtree::~Quadtree()
{
}

void Quadtree::Insert(GameObject * gameObject)
{
	if (QT_Box.Intersects(gameObject->boundingBox))
	{
		LOG("one to push");

		if (children.size() > 0)
		{
			for (int i = 0; i < children.size(); i++)
			{
				children[i]->Insert(gameObject);
			}
		}
		else
		{
			container.push_back(gameObject);
			
			if (container.size() > maxElements)
			{
				if (children.size() <= 0)
				{
					Split();
				}

				for (int j = 0; j < container.size(); j++)
				{
					for (int i = 0; i < children.size(); i++)
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

	x = QT_Box.MaxX() - QT_Box.MinX();
	y = QT_Box.MaxY() - QT_Box.MinY();
	z = QT_Box.MaxZ() - QT_Box.MinZ();

	vec diagonal = { x / 2,y,z / 2 };

	//first Child
	AABB first({ QT_Box.minPoint }, { QT_Box.minPoint + diagonal });
	children.push_back(new Quadtree(first));

	//second Child
	vec secondMin = QT_Box.minPoint + vec(x / 2, 0.0f, 0.0f);
	AABB second( secondMin ,  secondMin + diagonal );
	children.push_back(new Quadtree(second));

	//third Child
	vec thirdMin = QT_Box.minPoint + vec(0.0f, 0.0f, z/2);
	AABB third(thirdMin, thirdMin + diagonal);
	children.push_back(new Quadtree(third));

	//fourth Child
	AABB fourth(QT_Box.maxPoint-diagonal, QT_Box.maxPoint);
	children.push_back(new Quadtree(fourth));

}

void Quadtree::Remove(GameObject * gameObject)
{
}

void Quadtree::Intersect(vector<GameObject*>& gameObjects, const AABB & boundingBox)
{
	if (QT_Box.Intersects(boundingBox))
	{
		if (children.size() <= 0)
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

void Quadtree::Clear()
{
}

void Quadtree::DrawQT()
{
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->DrawQT();
	}

	glLineWidth(2.5f);
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);

	glVertex3f(QT_Box.CornerPoint(0).x, QT_Box.CornerPoint(0).y, QT_Box.CornerPoint(0).z);
	glVertex3f(QT_Box.CornerPoint(1).x, QT_Box.CornerPoint(1).y, QT_Box.CornerPoint(1).z);

	glVertex3f(QT_Box.CornerPoint(0).x, QT_Box.CornerPoint(0).y, QT_Box.CornerPoint(0).z);
	glVertex3f(QT_Box.CornerPoint(2).x, QT_Box.CornerPoint(2).y, QT_Box.CornerPoint(2).z);

	glVertex3f(QT_Box.CornerPoint(0).x, QT_Box.CornerPoint(0).y, QT_Box.CornerPoint(0).z);
	glVertex3f(QT_Box.CornerPoint(4).x, QT_Box.CornerPoint(4).y, QT_Box.CornerPoint(4).z);

	glVertex3f(QT_Box.CornerPoint(3).x, QT_Box.CornerPoint(3).y, QT_Box.CornerPoint(3).z);
	glVertex3f(QT_Box.CornerPoint(1).x, QT_Box.CornerPoint(1).y, QT_Box.CornerPoint(1).z);

	glVertex3f(QT_Box.CornerPoint(3).x, QT_Box.CornerPoint(3).y, QT_Box.CornerPoint(3).z);
	glVertex3f(QT_Box.CornerPoint(2).x, QT_Box.CornerPoint(2).y, QT_Box.CornerPoint(2).z);

	glVertex3f(QT_Box.CornerPoint(3).x, QT_Box.CornerPoint(3).y, QT_Box.CornerPoint(3).z);
	glVertex3f(QT_Box.CornerPoint(7).x, QT_Box.CornerPoint(7).y, QT_Box.CornerPoint(7).z);

	glVertex3f(QT_Box.CornerPoint(6).x, QT_Box.CornerPoint(6).y, QT_Box.CornerPoint(6).z);
	glVertex3f(QT_Box.CornerPoint(2).x, QT_Box.CornerPoint(2).y, QT_Box.CornerPoint(2).z);

	glVertex3f(QT_Box.CornerPoint(6).x, QT_Box.CornerPoint(6).y, QT_Box.CornerPoint(6).z);
	glVertex3f(QT_Box.CornerPoint(4).x, QT_Box.CornerPoint(4).y, QT_Box.CornerPoint(4).z);

	glVertex3f(QT_Box.CornerPoint(6).x, QT_Box.CornerPoint(6).y, QT_Box.CornerPoint(6).z);
	glVertex3f(QT_Box.CornerPoint(7).x, QT_Box.CornerPoint(7).y, QT_Box.CornerPoint(7).z);

	glVertex3f(QT_Box.CornerPoint(5).x, QT_Box.CornerPoint(5).y, QT_Box.CornerPoint(5).z);
	glVertex3f(QT_Box.CornerPoint(1).x, QT_Box.CornerPoint(1).y, QT_Box.CornerPoint(1).z);

	glVertex3f(QT_Box.CornerPoint(5).x, QT_Box.CornerPoint(5).y, QT_Box.CornerPoint(5).z);
	glVertex3f(QT_Box.CornerPoint(4).x, QT_Box.CornerPoint(4).y, QT_Box.CornerPoint(4).z);

	glVertex3f(QT_Box.CornerPoint(5).x, QT_Box.CornerPoint(5).y, QT_Box.CornerPoint(5).z);
	glVertex3f(QT_Box.CornerPoint(7).x, QT_Box.CornerPoint(7).y, QT_Box.CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);
}

