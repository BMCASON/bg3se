#include "stdafx.h"
#include "HttpFetcher.h"
#include "GameHelpers.h"
#include <iomanip>

BEGIN_SE()

HttpFetcher::HttpFetcher()
{}

HttpFetcher::~HttpFetcher()
{}

void HttpFetcher::LogError(CURL* curl, CURLcode result)
{
	std::stringstream ss;

	if (result == CURLE_HTTP_RETURNED_ERROR) {
		lastHttpCode_ = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lastHttpCode_);
		ss << "HTTP error " << lastHttpCode_;
	} else {
		ss << "(" << (long)result << ") " << curl_easy_strerror(result);
	}

	lastError_ = ss.str();
	DEBUG("Updater error: %s", lastError_.c_str());
}

bool HttpFetcher::Fetch(std::string const& url, std::vector<uint8_t> & response)
{
	auto curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_NOBODY, 0);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 10000);
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

	if (DebugLogging) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &DebugFunc);
	}

	if (IPv4Only) {
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	lastResponse_.clear();

	lastResult_ = curl_easy_perform(curl);
	if (lastResult_ != CURLE_OK) {
		LogError(curl, lastResult_);
	}

	curl_easy_cleanup(curl);
	response = lastResponse_;
	return (lastResult_ == CURLE_OK);
}

size_t HttpFetcher::WriteFunc(char* contents, size_t size, size_t nmemb, HttpFetcher* self)
{
	auto pos = self->lastResponse_.size();
	self->lastResponse_.resize(self->lastResponse_.size() + size * nmemb);
	memcpy(self->lastResponse_.data() + pos, contents, size * nmemb);
	return size * nmemb;
}

int HttpFetcher::DebugFunc(CURL* handle, curl_infotype type, char* data, size_t size, void* clientp)
{
	std::string line;
	switch (type) {
	case CURLINFO_TEXT: line = "* "; break;
	case CURLINFO_HEADER_IN: line = "< "; break;
	case CURLINFO_HEADER_OUT: line = "> "; break;
	default: return 0;
	}

	line += std::string_view(data, size - 2);
	DEBUG("%s", line.c_str());

	return 0;
}

END_SE()
