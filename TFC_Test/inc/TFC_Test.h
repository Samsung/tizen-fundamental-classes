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

void sync_thread(void(*func)(void*), void*);

void attach_widget(Evas_Object* obj);

#define EFL_BLOCK_BEGIN \
{\
	std::mutex tfc__mtx;\
	tfc__mtx.lock();\

#define EFL_SYNC_BEGIN(SHARED_DATA) \
struct tfc__sharedStruct { std::mutex& mtx; decltype(SHARED_DATA)& shared; } \
tfc__sharedInst = { tfc__mtx, SHARED_DATA };	\
sync_thread([] (void* tfc__data) {\
	typedef decltype(SHARED_DATA) tfc__sharedType;\
	tfc__sharedStruct* tfc__sharedData = reinterpret_cast<tfc__sharedStruct*>(tfc__data);\
	tfc__sharedType& SHARED_DATA = tfc__sharedData->shared;

#define EFL_SYNC_END \
	tfc__sharedData->mtx.unlock();\
}, &tfc__sharedInst);\
std::lock_guard<std::mutex> tfc__guard(tfc__mtx);

#define EFL_BLOCK_END \
}

#endif /* __TFC_Test_H__ */
