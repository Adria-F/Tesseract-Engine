#include "PanelAnimationGraph.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "ModuleResource.h"
#include "ModuleAnimations.h"
#include "Resource.h"
#include "ResourceAnimation.h"
#include "ResourceAnimationGraph.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentAnimation.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

PanelAnimationGraph::PanelAnimationGraph(const char* name) : Panel(name)
{
	active = true;
	showGrid = true;

	/*addNode("Test 1");
	addNode("Test 2", { 400,200 });*/
}

PanelAnimationGraph::~PanelAnimationGraph()
{
}

void PanelAnimationGraph::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	clickedLine = false;

	//Get the graph resource of the current selected game object
	bool hasGraph = false;
	if (App->scene_intro->selected_GO != nullptr)
	{
		component = (ComponentAnimation*)App->scene_intro->selected_GO->GetComponent(ANIMATION);
		if (component != nullptr)
		{
			graph = (ResourceAnimationGraph*)App->resources->GetResource(component->RUID);
			if (graph != nullptr && graph->GetType() == R_ANIMATION)
				graph = nullptr;
			else
				hasGraph = true;
		}
	}
	if (!hasGraph)
		graph = nullptr;

	// Create the child canvas
	if (ImGui::Button("Save Graph") && graph != nullptr)
	{
		App->animations->saveGraph(graph, graph->GetUID());
	}
	ImGui::SameLine();
	ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	ImGui::SameLine(ImGui::GetWindowWidth() - 100);
	ImGui::Checkbox("Show grid", &showGrid);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImU32 bckgColor = IM_COL32(50, 50, 55, 200);
	if (!hasGraph)
		bckgColor = IM_COL32(25, 25, 25, 200);
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, bckgColor);
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushItemWidth(120.0f);

	ImVec2 offset = { ImGui::GetCursorScreenPos().x + scrolling.x, ImGui::GetCursorScreenPos().y + scrolling.y };
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	// Display grid
	if (showGrid)
	{
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		if (!hasGraph)
			GRID_COLOR = IM_COL32(200, 200, 200, 15);
		float GRID_SZ = 64.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x+win_pos.x, win_pos.y), ImVec2(x+win_pos.x, canvas_sz.y+win_pos.y), GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(win_pos.x, y+win_pos.y), ImVec2(canvas_sz.x+win_pos.x, y+win_pos.y), GRID_COLOR);
	}

	if (graph == nullptr)
	{
		ImGui::PopItemWidth();
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		ImGui::End();
		return;
	}

	//Click to add nodes
	if (ImGui::IsMouseClicked(1))
	{
		graph->addNode("Test", { ImGui::GetMousePos().x-offset.x, ImGui::GetMousePos().y -offset.y});
	}

	//Draw content
	draw_list->ChannelsSplit(2); //Split current channel into layers
	draw_list->ChannelsSetCurrent(0);
	for (std::map<uint, Node*>::iterator it_n = graph->nodes.begin(); it_n != graph->nodes.end(); ++it_n)
	{
		Node* node = (*it_n).second;
		ImGui::PushID(node->UID);
		node->gridPos = { node->pos.x + offset.x,node->pos.y + offset.y };

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		//Node box
		ImU32 borderColor = IM_COL32(100, 100, 100, 255);
		ImU32 backgroundColor = IM_COL32(45, 45, 45, 230);

		if (hovered_node == node)
		{
			borderColor = IM_COL32(150, 150, 100, 255);
		}
		if (selected_node == node)
		{
			borderColor = IM_COL32(255, 255, 75, 255);
			backgroundColor = IM_COL32(55, 55, 55, 245);
		}

		draw_list->AddRectFilled({ node->gridPos.x, node->gridPos.y }, { node->gridPos.x + node->size.x, node->gridPos.y + node->size.y }, backgroundColor, 4.0f);
		draw_list->AddRect({ node->gridPos.x, node->gridPos.y }, { node->gridPos.x + node->size.x, node->gridPos.y + node->size.y }, borderColor, 4.0f, 15, (selected_node == node) ? 2.0f : 1.0f);

		//Node content
		ImGui::SetCursorScreenPos({ node->gridPos.x + GRAPH_NODE_WINDOW_PADDING, node->gridPos.y + GRAPH_NODE_WINDOW_PADDING });
		ImGui::BeginGroup();
		ImGui::Text(node->name.c_str());

		drawAnimationBox(node);
		uint clickedLink = node->drawLinks();
		if (clickedLink != 0)
			linkingNode = clickedLink;
		for (std::list<Transition*>::iterator it_t = node->transitions.begin(); it_t != node->transitions.end(); ++it_t)
		{
			if ((*it_t)->drawLine((*it_t) == selected_transition, { offset.x, offset.y }))
			{
				selected_transition = (*it_t);
				clickedLine = true;
			}
		}

		//Node selection & linking
		ImGui::SetCursorScreenPos({ node->gridPos.x, node->gridPos.y });
		ImGui::InvisibleButton("node", { node->size.x, node->size.y });
		if (ImGui::IsItemHovered())
		{
			hovered_node = node;

			if (linkingNode != 0 && !ImGui::IsMouseDown(0))
			{				
				NodeLink* linking = graph->getLink(linkingNode);
				if (linking != nullptr && linking->nodeUID != node->UID)
				{
					Node* originNode = graph->getNode(linking->nodeUID);
					if (originNode != nullptr && !originNode->checkLink(node))
					{
						NodeLink* linked = node->addLink(INPUT_LINK);
						linking->connect(linked->UID);
						linked->connect(linking->UID);

						originNode->connectLink(linkingNode);
						originNode->transitions.push_back(new Transition(linking, linked, graph->GetUID()));
					}
				}
				linkingNode = 0;
			}
		}
		if (linkingNode == 0 && ImGui::IsItemClicked())
		{
			selectNode(node);
			selected_transition = nullptr;
			dragging_node = node;
			clickOffset = { ImGui::GetMousePos().x - offset.x - node->pos.x, ImGui::GetMousePos().y - offset.y - node->pos.y };
		}

		ImGui::EndGroup();

		ImGui::PopID();
	}

	//Draw transition window
	if (selected_transition != nullptr)
	{
		draw_list->ChannelsSetCurrent(1);
		//Draw box
		float2 boxSize = { 190,300 };
		ImVec2 posA = { ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - boxSize.x - 5, ImGui::GetWindowPos().y + 5 };
		ImVec2 posB = { ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 5, ImGui::GetWindowPos().y + boxSize.y+5 };
		draw_list->AddRectFilled(posA, posB, IM_COL32(150, 150, 150, 255), 7.5f);
		draw_list->AddRect(posA, posB, IM_COL32(200, 200, 200, 255), 7.5f);

		//Draw box content
		ImGui::SetCursorScreenPos({ ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - boxSize.x + 5, ImGui::GetWindowPos().y + 15 });
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Text, { 0,0,0,255 });
		ImGui::Text("Transition");
		ImGui::Text("Do transtion when pressing:");
		if (ImGui::BeginCombo("Button", selected_transition->usingLetter.c_str()))
		{
			bool selected = false;
			ImGui::PushStyleColor(ImGuiCol_Text, { 255,255,255,255 });
			for (int i = 48; i <= 90; ++i)
			{
				if (i == 58)
					i = 65;
				char letter = i;
				std::string str = &letter;
				str = str.substr(0, 1);
				if (str == selected_transition->usingLetter)
					selected = true;
				else
					selected = false;
				if (ImGui::Selectable(str.c_str(), &selected))
				{
					selected_transition->usingLetter = str;
					if (i == 48)
						selected_transition->sdlKeyValue = 39;
					else if (i > 48 && i < 58)
						selected_transition->sdlKeyValue = i - 19;
					else if (i >= 65)
						selected_transition->sdlKeyValue = i - 61;
				}
			}
			ImGui::PopStyleColor();
			ImGui::EndCombo();
		}
		ImGui::PopStyleColor();

		ImGui::SetCursorScreenPos({ ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - boxSize.x - 5, ImGui::GetWindowPos().y + 5 });
		ImGui::InvisibleButton("transitionBox", { boxSize.x,boxSize.y });
		ImGui::EndGroup();
	}
	draw_list->ChannelsMerge();

	//Drag nodes
	if (!ImGui::IsMouseDown(0))
	{
		dragging_node = nullptr;
		linkingNode = 0;
	}
	if (!ImGui::IsAnyItemHovered())
	{
		hovered_node = nullptr;
		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
			selectNode(nullptr);
	}
	if (!clickedLine && ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
	{
		selected_transition = nullptr;
	}
	if (dragging_node != nullptr && ImGui::IsMouseDragging(0))
	{
		dragging_node->pos = { ImGui::GetMousePos().x-offset.x - clickOffset.x, ImGui::GetMousePos().y - offset.y-clickOffset.y };
	}

	//Drag window
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = { scrolling.x + ImGui::GetIO().MouseDelta.x, scrolling.y + ImGui::GetIO().MouseDelta.y };

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	ImGui::End();
}

void PanelAnimationGraph::selectNode(Node * node)
{
	selected_node = node;
}

void PanelAnimationGraph::drawAnimationBox(Node* node) const
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 pos = ImGui::GetCursorScreenPos();
	pos.y += 10.0f;

	draw_list->AddRectFilled(pos, { pos.x + 120, pos.y + 20 }, IM_COL32(25, 25, 25, 255), 1.0f);
	draw_list->AddRect(pos, { pos.x + 120, pos.y + 20 }, IM_COL32(150, 150, 150, 255), 1.0f, 15, 2.0f);

	ImGui::SetCursorScreenPos({ pos.x + GRAPH_NODE_WINDOW_PADDING, pos.y + GRAPH_NODE_WINDOW_PADDING/2 });

	ResourceAnimation* anim = (ResourceAnimation*)App->resources->GetResource(node->animationUID);
	ImGui::Text((anim == nullptr)? "No Animation":anim->GetName());

	ImGui::SetCursorScreenPos(pos);
	if (ImGui::InvisibleButton("animButton", { 120,20 }))
	{
		App->gui->startResourceList(R_ANIMATION, pos.x + 120, pos.y, component, std::to_string(node->UID).c_str());
	}
}