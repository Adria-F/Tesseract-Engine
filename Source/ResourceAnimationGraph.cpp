#include "ResourceAnimationGraph.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"
#include "PanelAnimationGraph.h"

inline static float GetSquaredDistanceToBezierCurve(const ImVec2& point, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4);
inline bool containPoint(ImVec2 A, ImVec2 B, ImVec2 point);

ResourceAnimationGraph::ResourceAnimationGraph(uint UID, ResType type) :Resource(UID, type)
{
}

ResourceAnimationGraph::~ResourceAnimationGraph()
{
}

bool ResourceAnimationGraph::LoadInMemory()
{
	return LoadGraph();
}

bool ResourceAnimationGraph::LoadGraph()
{
	//Get the buffer
	char* buffer = nullptr;
	App->fileSystem->readFile(exported_file.c_str(), &buffer);
	if (buffer == nullptr)
	{
		return false;
	}
	char* cursor = buffer;

	uint bytes = 0;

	int nodeAmount;
	memcpy(&nodeAmount, cursor, sizeof(int));
	cursor += sizeof(int);

	for (int i = 0; i < nodeAmount; ++i)
	{
		uint nameLength;
		memcpy(&nameLength, cursor, sizeof(uint));
		cursor += sizeof(uint);

		bytes = nameLength;
		char* auxName = new char[bytes];
		memcpy(auxName, cursor, bytes);
		std::string name = auxName;
		name = name.substr(0, bytes);
		RELEASE_ARRAY(auxName);
		cursor += bytes;

		float position[2];
		bytes = sizeof(float) * 2;
		memcpy(position, cursor, bytes);
		cursor += bytes;

		uint uids[3];
		bytes = sizeof(uint) * 3;
		memcpy(uids, cursor, bytes);
		cursor += bytes;
		
		Node* node = new Node(name.c_str(), this->UID, { position[0], position[1] }, uids[0]);
		node->animationUID = uids[1];

		for (int j = 0; j < uids[2]; ++j)
		{
			int type;
			bytes = sizeof(int);
			memcpy(&type, cursor, bytes);
			cursor += bytes;

			uint links[2];
			bytes = sizeof(uint) * 2;
			memcpy(links, cursor, bytes);
			cursor += bytes;

			NodeLink* link = node->addLink((linkType)type, true, links[0]);
			link->connect(links[1]);
		}
		nodes.insert(std::pair<uint, Node*>(uids[0], node));
	}

	for (std::map<uint, Node*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		Node* node = (*it_n).second;

		for (std::list<NodeLink*>::iterator it_l = node->links.begin(); it_l != node->links.end(); ++it_l)
		{
			if ((*it_l)->type == OUTPUT_LINK)
			{
				NodeLink* linked = getLink((*it_l)->connectedNodeLink);
				if (linked != nullptr)
				{
					node->transitions.push_back(new Transition((*it_l), linked, UID));
				}
			}
		}
	}

	RELEASE_ARRAY(buffer);
	return true;
}

bool ResourceAnimationGraph::UnloadFromMemory()
{
	for (std::map<uint, Node*>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
	{
		RELEASE((*it_n).second);
	}
	nodes.clear();
	links.clear(); //Links are deleted from node destructor

	return false;
}

Node* ResourceAnimationGraph::addNode(const char* name, float2 pos)
{
	Node* node = new Node(name, UID, pos);

	nodes.insert(std::pair<uint, Node*>(node->UID, node));

	return node;
}

void ResourceAnimationGraph::pushLink(NodeLink* link)
{
	links.insert(std::pair<uint, NodeLink*>(link->UID, link));
}

Node* ResourceAnimationGraph::getNode(uint UID)
{
	if (nodes.find(UID) != nodes.end())
		return nodes[UID];

	return nullptr;
}

NodeLink* ResourceAnimationGraph::getLink(uint UID)
{
	if (links.find(UID) != links.end())
		return links[UID];

	return nullptr;
}

Node::Node(const char* name, uint graphUID, float2 pos, float2 size): name(name), pos(pos), size(size), UID(GENERATE_UID()), graphUID(graphUID)
{
	addLink(OUTPUT_LINK);
}

Node::Node(const char * name, uint graphUID, float2 pos, uint forced_UID) : name(name), pos(pos), size({ 150,80 }), UID(forced_UID), graphUID(graphUID)
{
}

Node::~Node()
{
	for (std::list<NodeLink*>::iterator it_l = links.begin(); it_l != links.end(); ++it_l)
	{
		RELEASE((*it_l));
	}
	links.clear();
	for (std::list<Transition*>::iterator it_t = transitions.begin(); it_t != transitions.end(); ++it_t)
	{
		RELEASE((*it_t));
	}
	transitions.clear();
}

NodeLink* Node::addLink(linkType type, bool addToList, uint forced_uid)
{
	NodeLink* ret = new NodeLink(type, UID);
	if (forced_uid != 0)
		ret->UID = forced_uid;
	
	((ResourceAnimationGraph*)App->resources->GetResource(graphUID))->pushLink(ret);
	if (addToList)
		links.push_back(ret);

	switch (type)
	{
	case INPUT_LINK:
		++inputCount;
		break;
	case OUTPUT_LINK:
		++outputCount;
		break;
	}

	int maxCount = (inputCount > outputCount) ? inputCount : outputCount;
	int height = GRAPH_NODE_WINDOW_PADDING * 2 + GRAPH_LINK_RADIUS * 3 * maxCount;
	if (size.y < height)
		size.y = height;

	return ret;
}

uint Node::drawLinks() const
{
	uint ret = 0;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ResourceAnimationGraph* graph = (ResourceAnimationGraph*)App->resources->GetResource(graphUID);

	int OUTcount = 0, INcount = 0;

	for (std::list<NodeLink*>::const_iterator it_l = links.begin(); it_l != links.end(); ++it_l)
	{
		switch ((*it_l)->type)
		{
		case INPUT_LINK:
			++INcount;
			(*it_l)->nodePos = { 0, ((size.y / inputCount)*INcount) - ((size.y / inputCount) / 2) };
			break;
		case OUTPUT_LINK:
			++OUTcount;
			(*it_l)->nodePos = { size.x, ((size.y / outputCount)*OUTcount) - ((size.y / outputCount) / 2) };
			break;
		}
		ImVec2 linkPos = { gridPos.x + (*it_l)->nodePos.x, gridPos.y + (*it_l)->nodePos.y };

		draw_list->AddCircleFilled(linkPos, GRAPH_LINK_RADIUS, IM_COL32(25, 25, 25, 255));
		draw_list->AddCircle(linkPos, GRAPH_LINK_RADIUS, IM_COL32(130, 130, 130, 255), 12, 1.5f);
		if ((*it_l)->connectedNodeLink != 0)
			draw_list->AddCircleFilled(linkPos, GRAPH_LINK_RADIUS / 2, IM_COL32(130, 130, 130, 255));

		//Draw line
		float2 mousePos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
		if ((*it_l)->type == OUTPUT_LINK && (*it_l)->connectedNodeLink == 0 && ImGui::IsMouseClicked(0) && mousePos.Distance({ linkPos.x, linkPos.y }) <= GRAPH_LINK_RADIUS)
		{
			(*it_l)->linking = true;
			ret = (*it_l)->UID;
		}
		if ((*it_l)->linking)
		{
			draw_list->ChannelsSetCurrent(1);
			draw_list->AddBezierCurve(linkPos, { linkPos.x + (((*it_l)->type == OUTPUT_LINK) ? 50.0f : -50.0f), linkPos.y }, { mousePos.x - 50, mousePos.y }, { mousePos.x, mousePos.y }, IM_COL32(200, 200, 100, 255), 3.0f);
			draw_list->ChannelsSetCurrent(0);

			if (!ImGui::IsMouseDown(0))
				(*it_l)->linking = false;
		}
	}

	return ret;
}

bool Node::checkLink(Node* node)
{
	bool ret = false;

	for (std::list<Transition*>::iterator it_t = transitions.begin(); it_t != transitions.end(); ++it_t)
	{
		if ((*it_t)->destination == node)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void Node::connectLink(uint linkUID)
{
	NodeLink* prev = nullptr;
	for (std::list<NodeLink*>::iterator it_l = links.begin(); it_l != links.end(); ++it_l)
	{
		if ((*it_l)->UID == linkUID)
		{
			if (prev == nullptr || prev->connectedNodeLink != 0)
			{
				//Create link	
				NodeLink* added = addLink(OUTPUT_LINK, false);
				if (prev == nullptr)
					links.push_front(added);
				else
					links.insert(it_l, added);
			}
			if (++it_l == links.end() || (*it_l)->connectedNodeLink != 0)
			{
				//Create link
				NodeLink* added = addLink(OUTPUT_LINK, false);
				if (it_l == links.end())
					links.push_back(added);
				else
					links.insert(it_l, added);
			}

			break;
		}
		prev = (*it_l);
	}
}

NodeLink::NodeLink(linkType type, uint nodeUID) : type(type), nodeUID(nodeUID), UID(GENERATE_UID())
{
}

Transition::Transition(NodeLink* output, NodeLink* input, uint graphUID) : output(output), input(input), graphUID(graphUID)
{
	ResourceAnimationGraph* graph = (ResourceAnimationGraph*)App->resources->GetResource(graphUID);

	origin = graph->getNode(output->nodeUID);
	destination = graph->getNode(input->nodeUID);
}

bool Transition::drawLine(bool selected, float2 offset)
{
	bool ret = false;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 originPos = { origin->gridPos.x + output->nodePos.x, origin->gridPos.y + output->nodePos.y };
	ImVec2 destinationPos = { destination->gridPos.x + input->nodePos.x, destination->gridPos.y + input->nodePos.y };

	float lineThickness = 3.0f;
	float triangleSize = 10.0f;
	ImU32 color = IM_COL32(200, 200, 100, 255);
	if (selected)
	{
		color = IM_COL32(240, 240, 175, 255);
		lineThickness = 5.0f;
		triangleSize = 15.0f;
	}
	
	draw_list->ChannelsSetCurrent(1);
	draw_list->AddBezierCurve(originPos, { originPos.x + 50.0f, originPos.y }, { destinationPos.x - 50.0f, destinationPos.y }, destinationPos, color, lineThickness);
	draw_list->AddTriangleFilled(destinationPos, { destinationPos.x - triangleSize, destinationPos.y + triangleSize/2 }, { destinationPos.x - triangleSize, destinationPos.y - triangleSize/2 }, color);
	draw_list->ChannelsSetCurrent(0);

	if (ImGui::IsMouseDown(0) && App->gui->animationGraph->linkingNode == 0 && containPoint(originPos, destinationPos, ImGui::GetMousePos()))
	{
		float distance = GetSquaredDistanceToBezierCurve(ImGui::GetMousePos(), originPos, { originPos.x + 50.0f, originPos.y }, { destinationPos.x - 50.0f, destinationPos.y }, destinationPos);
		if (distance <= 15.0f)
			ret = true;
	}

	return ret;
}

inline static float ImVec2Dot(const ImVec2& S1, const ImVec2& S2) { return (S1.x*S2.x + S1.y*S2.y); }

inline static float GetSquaredDistancePointSegment(const ImVec2& P, const ImVec2& S1, const ImVec2& S2) {
	const float l2 = (S1.x - S2.x)*(S1.x - S2.x) + (S1.y - S2.y)*(S1.y - S2.y);
	if (l2 < 0.0000001f) return (P.x - S2.x)*(P.x - S2.x) + (P.y - S2.y)*(P.y - S2.y);   // S1 == S2 case
	ImVec2 T({S2.x-S1.x,S2.y-S1.y});
	const float tf = ImVec2Dot({P.x-S1.x, P.y-S1.y}, T) / l2;
	const float minTf = 1.f < tf ? 1.f : tf;
	const float t = 0.f > minTf ? 0.f : minTf;
	T = {S1.x+T.x*t,S1.y+T.y*t};  // T = Projection on the segment
	return (P.x - T.x)*(P.x - T.x) + (P.y - T.y)*(P.y - T.y);
}

inline static float GetSquaredDistanceToBezierCurve(const ImVec2& point, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4) {
	static const int num_segments = 4;   // Num Sampling Points In between p1 and p4
	static bool firstRun = true;
	static ImVec4 weights[num_segments];

	if (firstRun) {
		// static init here
		IM_ASSERT(num_segments > 0);    // This are needed for computing distances: cannot be zero
		firstRun = false;
		for (int i = 1; i <= num_segments; i++) {
			float t = (float)i / (float)(num_segments + 1);
			float u = 1.0f - t;
			weights[i - 1].x = u * u*u;
			weights[i - 1].y = 3 * u*u*t;
			weights[i - 1].z = 3 * u*t*t;
			weights[i - 1].w = t * t*t;
		}
	}

	float minSquaredDistance = FLT_MAX, tmp;   // FLT_MAX is probably in <limits.h>
	ImVec2 L = p1, tp;
	for (int i = 0; i < num_segments; i++) {
		const ImVec4& W = weights[i];
		tp.x = W.x*p1.x + W.y*p2.x + W.z*p3.x + W.w*p4.x;
		tp.y = W.x*p1.y + W.y*p2.y + W.z*p3.y + W.w*p4.y;

		tmp = GetSquaredDistancePointSegment(point, L, tp);
		if (minSquaredDistance > tmp) minSquaredDistance = tmp;
		L = tp;
	}
	tp = p4;
	tmp = GetSquaredDistancePointSegment(point, L, tp);
	if (minSquaredDistance > tmp) minSquaredDistance = tmp;

	return minSquaredDistance;
}

inline bool containPoint(ImVec2 A, ImVec2 B, ImVec2 point)
{
	ImVec2 bottomLeft = { Min(A.x, B.x), Max(A.x, B.x) };
	ImVec2 topRight = { Max(A.x, B.x), Min(A.y, B.y) };

	return point.x > bottomLeft.x && point.x < topRight.x && point.y < bottomLeft.y && point.y > topRight.y;
}