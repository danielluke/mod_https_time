# mod_https_time

Inspired by https://web.archive.org/web/20190322171747/http://phk.freebsd.dk/time/20151129.html

## Build:
apxs -i -a -c mod_https_time.c

## Configure Apache:

	LoadModule https_time_module modules/mod_https_time.so
	<Location /.well-known/time>
		SetHandler https_time-handler
	</Location>
