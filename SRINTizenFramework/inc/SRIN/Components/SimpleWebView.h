/*
 * SimpleWebView.h
 *
 *  Created on: Mar 17, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#ifndef SRIN_COMP_SIMPLEWEBVIEW_H_
#define SRIN_COMP_SIMPLEWEBVIEW_H_

#include "SRIN/Components/ComponentBase.h"

namespace SRIN {
namespace Components {

class LIBAPI SimpleWebView: public ComponentBase
{
private:
	std::string data;
	Evas_Object* box;
	Evas_Object* boxPage;

	void AddParagraph(Evas_Object* boxPage, std::string& paragraph);
	void AddImage(std::string& url);
	void Render();
protected:
	virtual Evas_Object* CreateComponent(Evas_Object* root);


public:
	SimpleWebView();
	void SetHTMLData(const std::string& data);
};


}
}



#endif /* SIMPLEWEBVIEW_H_ */
