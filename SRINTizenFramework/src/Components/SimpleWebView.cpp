
/*
 * SimpleWebView.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@samsung.com)
 */

#include <EWebKit.h>
#include "SRIN/Framework/Application.h"
#include "SRIN/Components/SimpleWebView.h"
#include "SRIN/Net/ImageCache.h"
#include "SRIN/Async.h"
#include <regex>
#include <stack>
#include <math.h>

/* Regular Expression for tokenizing HTML tag
 * Group 1: Matches the closing tag
 * Group 2: The closing tag string
 * Group 3: Matches the opening tag OR singleton
 * Group 4: The opening tag string
 * Group 5: Matches the attributes within the tag
 * 		> Inside Group 5 (Use separate function to parse each attribute
 * 		> Group 6: Entire attribute
 * 		> Group 7: Attribute name
 * 		> Group 8: Attribute value
 * Group 9: Matches if it is an self-enclosing tag
 */

#define HTML_REGEX R"REGEX((<\/([a-z][a-z0-9]*)>)|(<([a-z][a-z0-9]*)((\s([a-z][a-z0-9]*)\=\"(.*?)\")*)(\s\/)?>))REGEX"
#define ATTRIBUTE_REGEX R"REGEX(([a-z][a-z0-9]*)\=\"(.*?)\")REGEX"
#define HTML_REGEX_CLOSINGTAG 		1
#define HTML_REGEX_CLOSINGTAGSTR 	2
#define HTML_REGEX_OPENINGTAG 		3
#define HTML_REGEX_OPENINGTAGSTR 	4
#define HTML_REGEX_ATTRIBUTES		5
#define HTML_REGEX_SELFENCLOSE		9

#define HTML_TAG_LINEBREAK 			"br"
#define HTML_TAG_IMAGE 				"img"
#define HTML_TAG_STRONG				"strong"
#define HTML_TAG_SPAN				"span"
#define HTML_TAG_EM					"em"
#define HTML_TAG_P					"p"
#define HTML_TAG_TABLE				"table"

#define FILE_EDC_WEBVIEW "edc/SimpleWebView.edj"
#define EWK_FONT_SIZE 48

bool iequals(const std::string& a, const std::string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

std::string GetImageURL(std::string attributeList)
{
	std::regex attrTag(ATTRIBUTE_REGEX);
	std::sregex_iterator words_begin(attributeList.begin(), attributeList.end(), attrTag);
	auto words_end = std::sregex_iterator();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i)
	{
		std::smatch match = *i;
		std::string attr = match[1];

		if(iequals(attr, "src"))
		{
			return match[2];
		}
	}

	return "";
}

using namespace SRIN;

Evas_Object* SRIN::Components::SimpleWebView::CreateComponent(Evas_Object* root)
{
	fontFormat = "<font=";
	fontFormat.append(font);
	fontFormat.append(" font_size=");
	fontFormat.append(std::to_string(fontSize));
	fontFormat.append(">");

	dlog_print(DLOG_INFO, "SRINFW-Parser", fontFormat.c_str());

	this->box = elm_box_add(root);
	evas_object_size_hint_weight_set(this->box, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(this->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	Render();

	return this->box;

	// Experimental EWK
	/*layout = elm_layout_add(root);
	auto edj_path = Framework::ApplicationBase::GetResourcePath(FILE_EDC_WEBVIEW);
	elm_layout_file_set(layout, edj_path.c_str(), "simple_web_view");
	evas_object_show(layout);

	bg = elm_bg_add(layout);
	elm_object_part_content_set(layout, "web_view", bg);
	evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_min_set(bg, NULL, 1);
	evas_object_show(bg);

	ewk = ewk_view_add(evas_object_evas_get(bg));
	evas_object_size_hint_weight_set(ewk, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ewk, EVAS_HINT_FILL, EVAS_HINT_FILL);
	ewk_settings_default_font_size_set(ewk_view_settings_get(ewk), EWK_FONT_SIZE);
	elm_object_part_content_set(bg, "overlay", ewk);
	evas_object_show(ewk);

	return layout;*/
}

void SRIN::Components::SimpleWebView::Render()
{
	if(!this->box)
		return;

	isRendering = true;

	// Clear the box
	elm_box_unpack(box, boxPage);
	evas_object_del(boxPage);
	boxPage = elm_box_add(this->box);
	evas_object_size_hint_weight_set(boxPage, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(boxPage, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_padding_set(boxPage, 0, 20);
	elm_box_pack_start(this->box, boxPage);
	evas_object_show(boxPage);

	std::stack<std::string> currentTag;

	std::stringstream buffer;

	std::regex htmlTag(HTML_REGEX);

	std::string& text = this->data;

	bool flushForCustomProcess = false;
	std::stringstream customProcessBuffer;
	std::string customProcessTag;

	std::sregex_iterator words_begin(text.begin(), text.end(), htmlTag);
	auto words_end = std::sregex_iterator();

	auto lastToken = text.cbegin();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
		std::smatch match = *i;

		if(match[HTML_REGEX_OPENINGTAG].matched)
		{
			if(!flushForCustomProcess)
			{
				auto tokenEnd = match[HTML_REGEX_OPENINGTAG].first;
				// Store string between tag to buffer
				auto strCurr = std::string(lastToken, tokenEnd);
				buffer << strCurr;

				// Move to next token
				lastToken = match[HTML_REGEX_OPENINGTAG].second;

				std::string tag = match[HTML_REGEX_OPENINGTAGSTR];
				dlog_print(DLOG_VERBOSE, "SRINFW-Parser", "%s OPEN %s",  strCurr.c_str(), tag.c_str());

				if(iequals(tag, HTML_TAG_LINEBREAK) || iequals(tag, HTML_TAG_IMAGE) || iequals(tag, HTML_TAG_P))
				{
					// Close the dangling tags if any
					while(!currentTag.empty())
					{
						auto& top = currentTag.top();
						if(iequals(top, HTML_TAG_STRONG))
						{
							buffer << "</b>";
						}
						else if(iequals(top, HTML_TAG_SPAN) || iequals(top, HTML_TAG_EM))
						{
							buffer << "</em>";
						}

						currentTag.pop();
					}

					// Push buffer to box if it is not empty

					if(buffer.tellp() != 0)
					{
						auto str = buffer.str();
						AddParagraph(boxPage, str);
						buffer.str("");
					}

					if(iequals(tag, HTML_TAG_IMAGE))
					{
						auto src = GetImageURL(match[HTML_REGEX_ATTRIBUTES]);
						AddImage(src);
					}
				}
				else if(iequals(tag, HTML_TAG_TABLE))
				{
					flushForCustomProcess = true;
					customProcessBuffer.clear();
					customProcessBuffer << match[HTML_REGEX_OPENINGTAG];
					customProcessTag = match[HTML_REGEX_OPENINGTAGSTR];
				}
				else if(!match[HTML_REGEX_SELFENCLOSE].matched)
				{
					if(iequals(tag, HTML_TAG_STRONG))
					{
						buffer << "<b>";
						currentTag.push(tag);
					}
					else if(iequals(tag, HTML_TAG_SPAN) || iequals(tag, HTML_TAG_EM))
					{
						buffer << "<em>";
						currentTag.push(tag);
					}
					else if(iequals(tag, HTML_TAG_P))
					{
						// Push buffer to box as it is a paragraph
						if(buffer.tellp() != 0)
						{
							auto str = buffer.str();
							AddParagraph(boxPage, str);
							buffer.str("");
						}

						currentTag.push(tag);
					}
				}
			}
			else
			{
				// Flush everything to custom process

				auto tokenEnd = match[HTML_REGEX_OPENINGTAG].first;
				// Store string between tag to buffer
				auto strCurr = std::string(lastToken, tokenEnd);
				customProcessBuffer << strCurr;

				// Move to next token
				lastToken = match[HTML_REGEX_OPENINGTAG].second;

				// Flush the tag
				customProcessBuffer << match[HTML_REGEX_OPENINGTAG];
			}
		}
		else if(match[HTML_REGEX_CLOSINGTAG].matched)
		{
			if(flushForCustomProcess)
			{
				auto tokenEnd = match[HTML_REGEX_CLOSINGTAG].first;

				// Store string between tag to buffer
				std::string token(lastToken, tokenEnd);
				customProcessBuffer << token << match[HTML_REGEX_CLOSINGTAG];

				if(customProcessTag == match[HTML_REGEX_CLOSINGTAGSTR])
				{
					// Call event for custom processing
					std::string customProcessingHtml = customProcessBuffer.str();
					this->eventProcessingCustomTag(this, &customProcessingHtml);
					flushForCustomProcess = false;
					customProcessTag = "";
					customProcessBuffer.clear();
				}

				lastToken = match[HTML_REGEX_CLOSINGTAG].second;
			}
			else
			{
				auto tokenEnd = match[HTML_REGEX_CLOSINGTAG].first;

				// Store string between tag to buffer
				std::string token(lastToken, tokenEnd);
				buffer << token;
				dlog_print(DLOG_VERBOSE, "SRINFW-Parser", "%s CLOSING %s", token.c_str(), match[HTML_REGEX_CLOSINGTAGSTR].str().c_str());
				if(!currentTag.empty())
				{
					auto& top = currentTag.top();
					if(iequals(match[HTML_REGEX_CLOSINGTAGSTR].str(), top))
					{
						// If it is correct closing tag, pop the tag and put closing tag in buffer
						if(iequals(top, HTML_TAG_STRONG))
						{
							buffer << "</b>";
						}
						else if(iequals(top, HTML_TAG_SPAN) || iequals(top, HTML_TAG_EM))
						{
							buffer << "</em>";
						}
						else if(iequals(top, HTML_TAG_P))
						{
							// Push buffer to box as it is a paragraph
							if(buffer.tellp() != 0)
							{
								auto str = buffer.str();
								AddParagraph(boxPage, str);
								buffer.str("");
							}
						}

						currentTag.pop();
						// Otherwise ignore
					}
				}
				lastToken = match[HTML_REGEX_CLOSINGTAG].second;
			}
		}
	 }

	// After all is parsed, check the remaining buffer behind

	std::string final(lastToken, text.cend());
	buffer << final;

	if(buffer.tellp() != 0)
	{
		// Close the dangling tags if any
		while(!currentTag.empty())
		{
			auto& top = currentTag.top();
			if(iequals(top, HTML_TAG_STRONG))
			{
				buffer << "</b>";
			}
			else if(iequals(top, HTML_TAG_SPAN) || iequals(top, HTML_TAG_EM))
			{
				buffer << "</em>";
			}

			currentTag.pop();
		}

		// Push buffer to box if it is not empty

		auto str = buffer.str();
		AddParagraph(boxPage, str);
		buffer.str("");
	}

	isRendering = false;
}

SRIN::Components::SimpleWebView::SimpleWebView() :
		Font(this), FontSize(this),
		box(nullptr), boxPage(nullptr)
{
	eventImageDownloadCompleted += { this, &SimpleWebView::OnImageDownloadCompleted };

	font = "Tizen";
	fontSize = SIMPLE_WEB_VIEW_FONT_SIZE;
	// Experimental EWK
	/*ewk_init();
	eventEwkLoadFinished += { this, &SimpleWebView::OnEwkLoadFinished };*/
}

void SRIN::Components::SimpleWebView::SetFont(const std::string& font)
{
	this->font = font;
}

std::string& SRIN::Components::SimpleWebView::GetFont()
{
	return font;
}

void SRIN::Components::SimpleWebView::SetFontSize(const int& fontSize)
{
	this->fontSize = fontSize;
}

int SRIN::Components::SimpleWebView::GetFontSize()
{
	return fontSize;
}

void SRIN::Components::SimpleWebView::AddParagraph(Evas_Object* boxPage, std::string& paragraph)
{
	auto textField = elm_entry_add(boxPage);
	evas_object_size_hint_weight_set(textField, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(textField, EVAS_HINT_FILL, EVAS_HINT_FILL);

	std::string formatted = fontFormat;
	formatted.append(paragraph);
	formatted.append("</font>");

	elm_object_text_set(textField, formatted.c_str());
	elm_entry_single_line_set(textField, EINA_FALSE);
	elm_entry_scrollable_set(textField, EINA_FALSE);
	elm_entry_line_wrap_set(textField, ELM_WRAP_MIXED);
	elm_entry_editable_set(textField, EINA_FALSE);
	evas_object_show(textField);
	elm_box_pack_end(boxPage, textField);
	dlog_print(DLOG_INFO, "SRINFW-Parser", "DUMP PAR %s", formatted.c_str());
}

void SRIN::Components::SimpleWebView::AddImage(std::string& url)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "WebView: Add image %s", url.c_str());
	std::string filePath;
	// If the cache will require downloading
	if(Net::ImageCache::RequireDownloading(url, filePath))
	{
		auto placeholder = elm_box_add(boxPage);
		evas_object_size_hint_weight_set(placeholder, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(placeholder, EVAS_HINT_FILL, 0);

		// Add reference counter to ensure the box will not become invalid until the asynchronous method is completed
		evas_object_ref(placeholder);
		evas_object_show(placeholder);
		elm_box_pack_end(boxPage, placeholder);

		// Load image asynchrounously
		dlog_print(DLOG_DEBUG, LOG_TAG, "WebView: Load image from URL");
		s_async [placeholder, url] () -> ImageAsyncPackage { return { placeholder, Net::ImageCache::LoadImage(url) }; } >> eventImageDownloadCompleted;
	}
	else
	{
		// File exist, set image
		dlog_print(DLOG_DEBUG, LOG_TAG, "WebView: File exist, setting image");
		auto image = elm_image_add(boxPage);
		elm_image_file_set(image, filePath.c_str(), NULL);
		elm_image_aspect_fixed_set(image, EINA_TRUE);
		elm_image_fill_outside_set(image, EINA_FALSE);



		struct P {
			Evas_Object* box;
			Evas_Object* image;
		}* p = new P({ boxPage, image });

		ecore_idler_add([] (void* data) -> Eina_Bool {
			auto p = reinterpret_cast<P*>(data);
			Evas_Coord x, y, w, h;
			evas_object_geometry_get(p->box, &x, &y, &w, &h);
			int imgW, imgH;
			elm_image_object_size_get(p->image, &imgW, &imgH);

			double aspect = double(imgH) / double(imgW);
			int finalH = w * aspect;
			dlog_print(DLOG_DEBUG, LOG_TAG, "Image: w=%d h=%d Placeholder w=%d h=%d Aspect %f", imgW, imgH, w, finalH, aspect);
			evas_object_size_hint_min_set (p->image, w, finalH);

			if(w == 0)
				return true;
			else
				return false;
		}, p);


		evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_display_mode_set(image, EVAS_DISPLAY_MODE_EXPAND);
		evas_object_show(image);
		elm_box_pack_end(boxPage, image);
	}
}

void SRIN::Components::SimpleWebView::SetHTMLData(const std::string& data)
{
	this->data = data;
	Render();

	// Experimental EWK
	/*ewk_view_contents_set(ewk, data.c_str(), data.size(), NULL, NULL, NULL);
	evas_object_smart_callback_add(ewk, "load,finished", &SmartEventHandler, &eventEwkLoadFinished);*/
}

/*void SRIN::Components::SimpleWebView::OnEwkLoadFinished(ElementaryEvent* viewSource, Evas_Object* objSource, void* eventData)
{
	Evas_Coord ewkWidth, ewkHeight, bgWidth, bgHeight;
	ewk_view_contents_size_get(ewk, &ewkWidth, &ewkHeight);
	dlog_print(DLOG_DEBUG, "WEBVIEW", "ewk : %d, %d", ewkWidth, ewkHeight);

	evas_object_geometry_get(bg, NULL, NULL, &bgWidth, &bgHeight);
	dlog_print(DLOG_DEBUG, "WEBVIEW", "bg : %d, %d", bgWidth, bgHeight);
	double optimumSize = ((double)bgWidth) / ewkWidth * ewkHeight;
	dlog_print(DLOG_DEBUG, "WEBVIEW", "optimum size : %f", optimumSize);
	evas_object_size_hint_min_set(bg, NULL, (int)(std::round(optimumSize)));
}*/

void SRIN::Components::SimpleWebView::OnImageDownloadCompleted(Async<ImageAsyncPackage>::BaseEvent* event,
	Async<ImageAsyncPackage>::Task* asyncTask, ImageAsyncPackage result)
{
	auto img = elm_image_add(result.placeholder);
	elm_image_aspect_fixed_set(img, EINA_TRUE);
	elm_image_fill_outside_set(img, EINA_FALSE);
	elm_image_file_set(img, result.filePath.c_str(), nullptr);

	Evas_Coord x, y, w, h;
	evas_object_geometry_get(boxPage, &x, &y, &w, &h);

	evas_object_size_hint_min_set (img, w, 400);

	evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);

	// Pack the created image to the box stored in ImageLoading structure
	elm_box_pack_end(result.placeholder, img);

	// Don't forget to show the image
	evas_object_show(img);

	evas_object_unref(result.placeholder);
}

SRIN::Components::SimpleWebView::~SimpleWebView()
{
	eventImageDownloadCompleted -= { this, &SimpleWebView::OnImageDownloadCompleted };

	// Experimental EWK
	/*ewk_shutdown();*/
}

void SRIN::Components::SimpleWebView::AddCustomComponent(
		Evas_Object* component) {
	if(this->isRendering)
	{
		elm_box_pack_end(this->boxPage, component);
		dlog_print(DLOG_VERBOSE, "SRINFW-Parser", "Custom Component");
	}
}
