/*
 * SimpleWebView.h
 *
 *  Created on: Mar 17, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMP_SIMPLEWEBVIEW_H_
#define SRIN_COMP_SIMPLEWEBVIEW_H_

#include "SRIN/Components/ComponentBase.h"
#include "SRIN/Async.h"

#define SIMPLE_WEB_VIEW_FONT_SIZE 32

namespace SRIN {
namespace Components {

class LIBAPI SimpleWebView: public ComponentBase
{
private:
	std::string data;
	Evas_Object* box;
	Evas_Object* boxPage;
	bool isRendering;

	std::string fontFormat;
	std::string font;
	int fontSize;

	void SetFont(const std::string& font);
	std::string& GetFont();

	void SetFontSize(const int& fontSize);
	int GetFontSize();

	// Experimental EWK
	//	Evas_Object* layout;
	//	Evas_Object* bg;
	//	Evas_Object* ewk;

	// ElementaryEvent eventEwkLoadFinished;
	// void OnEwkLoadFinished(ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData);

	void AddParagraph(Evas_Object* boxPage, std::string& paragraph);
	void AddImage(std::string& url);
	void Render();

	struct ImageAsyncPackage {
		Evas_Object* placeholder;
		std::string filePath;
	};

	Async<ImageAsyncPackage>::Event eventImageDownloadCompleted;

	void OnImageDownloadCompleted(Async<ImageAsyncPackage>::BaseEvent* event, Async<ImageAsyncPackage>::Task* asyncTask, ImageAsyncPackage result);
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root);


public:
	SimpleWebView();
	~SimpleWebView();
	void SetHTMLData(const std::string& data);
	void AddCustomComponent(Evas_Object* component);
	Event<SimpleWebView*, std::string*> eventProcessingCustomTag;

	Property<SimpleWebView, std::string&>::GetSet<&SimpleWebView::GetFont, &SimpleWebView::SetFont> Font;
	Property<SimpleWebView, int>::GetSet<&SimpleWebView::GetFontSize, &SimpleWebView::SetFontSize> FontSize;
};


}
}



#endif /* SIMPLEWEBVIEW_H_ */
