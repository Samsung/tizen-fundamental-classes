/*
 * SplashScreenView.cpp
 *
 *  Created on: Apr 11, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include "View/SplashScreenView.h"


$(appName)::View::SplashScreenView::SplashScreenView() :
	rootLayout(nullptr), labelText(nullptr),
	FieldText(fieldText.Text) // This is to make property Text of Field visible to the user of this class, in this case is the controlelr
{
}

$(appName)::View::SplashScreenView::~SplashScreenView()
{
}

Evas_Object* $(appName)::View::SplashScreenView::CreateView(Evas_Object* root)
{
	// Create primitive Elementary container
	this->rootLayout = elm_box_add(root);
	evas_object_size_hint_weight_set(this->rootLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->rootLayout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	// Create primitive Elementary label
	this->labelText = elm_label_add(this->rootLayout);
	evas_object_size_hint_weight_set(this->rootLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(this->rootLayout, EVAS_HINT_FILL, 0);
	elm_object_text_set(this->labelText, "Hello SRIN Framework");
	elm_box_pack_end(this->rootLayout, this->labelText);

	// Create framework component Field
	this->fieldText.Create(root); // Components act as factory object which handle the creation of the widget
	this->fieldText.BottomBorderVisible = true; // Component can have property that can be assigned just like a field
	elm_box_pack_end(this->rootLayout, this->fieldText); // Component can interact directly with primitive Elementary API

	evas_object_show(this->rootLayout);
	evas_object_show(this->labelText);
	// Components usually by default has been set to show. No need to call extra evas_object_show for component

	// Return primitive Elementary container to be attached to the view
	return rootLayout;
}
