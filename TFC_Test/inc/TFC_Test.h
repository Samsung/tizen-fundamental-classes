#ifndef __TFC_Test_H__
#define __TFC_Test_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "TFC_Test"

#if !defined(PACKAGE)
#define PACKAGE "org.example.tfc_test"
#endif

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
} appdata_s;

extern appdata_s ad;

void attach_widget(Evas_Object* obj);

#endif /* __TFC_Test_H__ */
