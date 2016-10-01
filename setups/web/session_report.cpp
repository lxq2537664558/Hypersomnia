#include "session_report.h"
#include "3rdparty/http/microhttpd.h"
#include <cstdio>
#include <string>

#include "augs/filesystem/file.h"

int get_origin(void *cls, enum MHD_ValueKind kind,
	const char *key, const char *value)
{
	std::string& target_val = *reinterpret_cast<std::string*>(cls);

	if (!strcmp(key, "Origin"))
		target_val = value;
	
	return MHD_YES;
}

static int
ahc_echo(void *cls,
	struct MHD_Connection *connection,
	const char *url,
	const char *method,
	const char *version,
	const char *upload_data, size_t *upload_data_size, void **ptr)
{
	static int aptr;

	session_report& rep = *reinterpret_cast<session_report*>(cls);
	std::string page = rep.prepend_html;
	
	{
		std::unique_lock<std::mutex> lock(rep.fetch_stats_mutex);
		
		page += rep.fetched_stats;
	}

	page += rep.append_html;

	struct MHD_Response *response;
	int ret;

	if (0 != strcmp(method, "GET"))
		return MHD_NO;              /* unexpected method */
	if (&aptr != *ptr)
	{
		/* do never respond on first call */
		*ptr = &aptr;
		return MHD_YES;
	}
	*ptr = NULL;                  /* reset when done */
	response = MHD_create_response_from_buffer(page.length(),
		(void *)page.c_str(),
		MHD_RESPMEM_MUST_COPY);

	std::string origin_value;
	MHD_get_connection_values(connection, MHD_HEADER_KIND, &get_origin, &origin_value);

	MHD_add_response_header(response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, origin_value.c_str());

	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	return ret;
}

bool session_report::start_daemon(const std::string session_report_html, const unsigned short port) {
	std::string contents = augs::get_file_contents(session_report_html);
	
	const std::string stats_token = "%stats%";

	auto it = contents.find(stats_token);

	prepend_html.assign(contents.begin(), contents.begin() + it);
	append_html.assign(contents.begin() + it+stats_token.length(), contents.end());

	d = MHD_start_daemon(// MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_POLL,
		MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
		// MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG | MHD_USE_POLL,
		// MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG,
		port,
		NULL, NULL, &ahc_echo, this,
		MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)120,
		MHD_OPTION_END);

	return d != NULL;
}

void session_report::stop_daemon() {
	MHD_stop_daemon(d);
}

void session_report::fetch_stats(const std::string new_stats) {
	std::unique_lock<std::mutex> lock(fetch_stats_mutex);

	fetched_stats = new_stats;
}
