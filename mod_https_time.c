/* mod_https_time.c: simple apache module to output X-HTTPSTIME header
   see also http://phk.freebsd.dk/time/20151129.html

Original author: Daniel J. Luke (dluke@geeklair.net)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <sys/time.h>
#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "apr_strings.h"
#include "http_protocol.h"

module AP_MODULE_DECLARE_DATA https_time;

static int https_time_handler(request_rec *r)
{
	struct timeval tv;

	if (!r->handler || strcmp(r->handler, "https_time-handler") || r->proto_num < 1001 ) return (DECLINED);
	
	/* Get the time */
	if( 0 != gettimeofday(&tv, NULL) )
	{
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, 
			"mod_https_time: gettimeofday() call failed.");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* Add the header */	
	apr_table_set(r->headers_out, "X-HTTPSTIME", 
		apr_psprintf(r->pool, "%ld.%06ld", (long)tv.tv_sec, (long)tv.tv_usec));

	/* Don't cache the time, please */
	/* Don't bother with Pragma: no-cache because we don't respond to HTTP 1.0 requests */
	apr_table_set(r->headers_out, "Cache-Control", "no-cache, no-store, must-revalidate");
	apr_table_set(r->headers_out, "Expires", "0");
	
	return OK;	
}

static void register_hooks(apr_pool_t *pool)
{
	ap_hook_handler(https_time_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

AP_DECLARE_MODULE(https_time) =
{
	STANDARD20_MODULE_STUFF,
	NULL,	/* dir config creater */
	NULL,	/* dir merger --- default is to override */
	NULL,	/* server config */
	NULL,	/* merge server configs */
	NULL,	/* command apr_table_t */
	register_hooks	/* register hooks */
};
