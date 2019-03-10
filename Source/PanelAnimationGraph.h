#ifndef __PANELANIMATIONGRAPH_H__
#define __PANELANIMATIONGRAPH_H__

#include "Panel.h"
#include <map>

struct Node;
struct Transition;
class ResourceAnimationGraph;
class ComponentAnimation;

class PanelAnimationGraph : public Panel
{
public:
	PanelAnimationGraph(const char* name);
	~PanelAnimationGraph();

	void Draw();
	void drawAnimationBox(Node* node) const;

	void selectNode(Node* node);	

public:

	ImVec2 scrolling = { 0.0f,0.0f };
	bool showGrid = true;
	uint linkingNode = 0;

private:

	ComponentAnimation* component = nullptr;
	ResourceAnimationGraph* graph = nullptr;

	Node* selected_node = nullptr;
	Node* hovered_node = nullptr;
	Node* dragging_node = nullptr;
	ImVec2 clickOffset = { 0.0f,0.0f };

	Transition* selected_transition = nullptr;
	bool clickedLine = false;
};

#endif // !__PANELANIMATIONGRAPH_H__