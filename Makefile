CC     = gcc
CFLAGS = -O2 -Wall

req_objects= http_parse_request.o D_linked_list.o common.o

res_objects= $(req_objects) http_generate_response.o file_loader.o

sev_objects = $(res_objects) conn_obj.o cgi.o server.o

default: server

.PHONY: default clean clobber

request: $(req_objects)
	$(CC) -o  $@ $^

response: $(res_objects)
	$(CC) -o  $@ $^

server: $(sev_objects)
	$(CC) -o  $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -g -o $@ $<

clean: cserver

crequest:
	rm -f request $(req_objects)

cresponse:
	rm -f response $(res_objects)

cserver:
	rm -f server $(sev_objects)
