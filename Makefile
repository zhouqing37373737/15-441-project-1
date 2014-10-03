CC     = gcc
CFLAGS = -O2 -Wall

req_objects= http_parse_request.o D_linked_list.o common.o

res_objects= $(req_objects) http_generate_response.o file_loader.o

objects = conn_obj.o cgi.o server.o http_generate_response.o file_loader.o D_linked_list.o http_parse_request.o

default: server

.PHONY: default clean clobber

request: $(req_objects)
	$(CC) -o  $@ $^

response: $(res_objects)
	$(CC) -o  $@ $^

server: $(objects)
	$(CC) -o  $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -g -o $@ $<

clean:
	rm -f $(objects)

crequest:
	rm -f request $(req_objects)

cresponse:
	rm -f request $(res_objects)

clobber: clean
	rm -f server
