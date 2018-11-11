#ifndef __PANELFILE_DIALOG_H__
#define __PANELFILE_DIALOG_H__

#include "Panel.h"

class PanelFileDialog : public Panel
{
public:
	PanelFileDialog(const char* name);
	~PanelFileDialog();

	void Draw();

	void SaveAt(const char* path, const char* exclusiveExtension = nullptr);
	void LoadAt(const char* path, const char* exclusiveExtension = nullptr);
	
	void clearElements();

public:

	std::string path = "";

	std::list<assetsElement*> elements;
	assetsElement* currElement = nullptr;
};

#endif // !__PANELFILE_DIALOG_H__