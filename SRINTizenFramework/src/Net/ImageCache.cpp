/*
 * ImageCache.cpp
 *
 *  Created on: Mar 14, 2016
 *      Contributor:
 *        Gilang M. Hamidy (g.hamidy@samsung.com)
 *        Kevin Winata (k.winata@samsung.com)
 */

#include "SRIN/Net/ImageCache.h"

#define __STDBOOL_H // Remove STDBOOL
#include <app.h>
#include <sqlite3.h>
#include <cstdlib>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>

#define DBNAME "ImageCache.db"

using namespace SRIN::Net;

bool staticInitialized = false;
std::string storagePath;
std::string dbPath;
sqlite3* db;
CString tableDdl = "CREATE TABLE IF NOT EXISTS ImageCache ("
						"id INTEGER PRIMARY KEY,"
						"url TEXT UNIQUE,"
						"filename TEXT"
					")";

CString queryImage = "SELECT id, filename FROM ImageCache WHERE url = ?";
CString queryInsert = "INSERT INTO ImageCache(url, filename) VALUES (?, ?)";

bool cacheInitFailed = false;

size_t ImageCache_WriteCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

bool ImageCache_EnsureStaticInitialized()
{
	if(!staticInitialized)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "ImageCache Init");
		staticInitialized = true;
		auto path = app_get_data_path();
		storagePath = path;
		free(path);


		struct stat st = {0};

		if (stat(storagePath.c_str(), &st) == -1) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "mkdir %s", storagePath.c_str());
		    auto ret = mkdir(storagePath.c_str(), 0755);
		    dlog_print(DLOG_DEBUG, LOG_TAG, "mkdir result %d", ret);
		}
		dbPath = storagePath + DBNAME;

		// Open database
		sqlite3_config(SQLITE_CONFIG_URI, 1);
		sqlite3_initialize();
		auto res = sqlite3_open(dbPath.c_str(), &db);

		if(res != SQLITE_OK)
		{
			cacheInitFailed = true;
			dlog_print(DLOG_DEBUG, LOG_TAG, "SQLite fail open %s code: %d", dbPath.c_str(), res);
			return false;
		}

		// Ensure it has the table to store cache table
		char* errMsg = nullptr;
		auto dbFailure = sqlite3_exec(db, tableDdl, NULL, 0, &errMsg);

		if(dbFailure)
		{
			dlog_print(DLOG_ERROR, LOG_TAG, "ImageCache Error: DB:52 => %s", errMsg);
			sqlite3_free(errMsg);
			errMsg = nullptr;

			cacheInitFailed = true;
			return false;
		}

		return true;
	}
	else
	{
		return !cacheInitFailed;
	}
}

std::string ImageCache_ExtractFilename(const std::string& url)
{
	std::string path;


	size_t sep = url.find_last_of("\\/");
	if (sep != std::string::npos)
		path = url.substr(sep + 1, url.size() - sep - 1);

	size_t dot = path.find_last_of(".");
	if (dot != std::string::npos)
	{
		std::string name = path.substr(0, dot);
		std::string ext  = path.substr(dot, path.size() - dot);

		size_t queryMark = ext.find_first_of("?");
		if(queryMark != std::string::npos)
		{
			ext = ext.substr(0, queryMark);
		}

		return name + ext;
	}
	else
	{
		std::string name = path;
		return name + "jpg";
	}
}

std::string LIBAPI SRIN::Net::ImageCache::LoadImage(const std::string& url)
{
	// Just return plain url if the cache init failed
	if(!ImageCache_EnsureStaticInitialized())
		return url;

	if(url.length() == 0)
		return "";

	std::string retFile;

	if(!RequireDownloading(url, retFile))
	{
		return retFile;
	}

	auto curlHandle = curl_easy_init();

	if (curlHandle) {
		// Include timestamp in the filename to prevent duplicates
		auto tp = std::chrono::system_clock::now();
		std::string fileName = ImageCache_ExtractFilename(url);
		fileName += std::to_string(tp.time_since_epoch().count());

		std::string filePath = storagePath + fileName;

		dlog_print(DLOG_DEBUG, LOG_TAG, "Try fopen file : %s", filePath.c_str());
		auto fp = fopen(filePath.c_str(), "wb");

		dlog_print(DLOG_DEBUG, LOG_TAG, "FOpen result %d", fp);
		curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, ImageCache_WriteCallback);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1);

		dlog_print(DLOG_DEBUG, LOG_TAG, "Starting download..");
		auto res = curl_easy_perform(curlHandle);
		fclose(fp);
		if(res != CURLE_OK) {
			dlog_print(DLOG_ERROR, LOG_TAG, "Download failed.");
		}
		else {
			long httpcode;
			curl_easy_getinfo (curlHandle, CURLINFO_RESPONSE_CODE, &httpcode);
			dlog_print(DLOG_DEBUG, LOG_TAG, "Download finished, HTTP code : %ld", httpcode);
			if(httpcode != 200){
				dlog_print(DLOG_DEBUG, LOG_TAG, "Invalid, removing file");
				unlink(filePath.c_str());
				filePath = url;
			} else {
				// Store to database
				sqlite3_stmt* statement;
				auto prepareResult = sqlite3_prepare(db, queryInsert, -1, &statement, nullptr);
				sqlite3_bind_text(statement, 1, strdup(url.c_str()), -1, [] (void* d) { free(d); });
				sqlite3_bind_text(statement, 2, strdup(fileName.c_str()), -1, [] (void* d) { free(d); });

				sqlite3_step(statement);

				dlog_print(DLOG_DEBUG, LOG_TAG, "Prepare result: %d", prepareResult);

				sqlite3_finalize(statement);
			}
		}
		/* always cleanup */
		curl_easy_cleanup(curlHandle);


		return filePath;
	}
	else
		return url;
}

bool LIBAPI SRIN::Net::ImageCache::RequireDownloading(const std::string& url, std::string& filePath)
{
	if(!ImageCache_EnsureStaticInitialized())
	{
		filePath = url;
		return false;
	}

	sqlite3_stmt* statement;
	sqlite3_prepare(db, queryImage, -1, &statement, nullptr);
	sqlite3_bind_text(statement, 1, strdup(url.c_str()), -1, [] (void* d) { free(d); });
	auto queryResult = sqlite3_step(statement);
	bool returnValue = false;

	if(queryResult == SQLITE_DONE)
	{
		returnValue = true;
	}
	else if(queryResult == SQLITE_ROW)
	{
		filePath = storagePath;
		filePath.append((CString)sqlite3_column_text(statement, 1));
		returnValue = false;
	}

	sqlite3_finalize(statement);
	return returnValue;
}
