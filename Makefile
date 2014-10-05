CC     = gcc
CFLAGS = -O2 -Wall

req_objects= http_parse_request.o D_linked_list.o common.o

res_objects= $(req_objects) http_generate_response.o file_loader.o

sev_objects = $(res_objects) conn_obj.o cgi.o server.o ssl.o

lisod_objects = $(sev_objects) lisod.o logger.o
default: lisod 

.PHONY: default clean clobber

request: $(req_objects)
	$(CC) -o  $@ $^

response: $(res_objects)
	$(CC) -o  $@ $^

server: $(sev_objects)
	$(CC) -o  $@ $^ -lcrypto -lssl

lisod: $(lisod_objects)
	$(CC) -o  $@ $^ -lcrypto -lssl
%.o: %.c
	$(CC) -c $(CFLAGS) -g -o $@ $<

clean: clisod

crequest:
	rm -f request $(req_objects)

cresponse:
	rm -f response $(res_objects)

cserver:
	rm -f server $(sev_objects)
clisod:
	rm -f lisod log.txt lock.txt $(lisod_objects)
