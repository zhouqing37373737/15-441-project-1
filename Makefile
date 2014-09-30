CC     = gcc
CFLAGS = -O2 -Wall

objects = http_generate_response.o file_loader.o D_linked_list.o http_parse_request.o

default: http_generate_response

.PHONY: default clean clobber

http_generate_response: $(objects)
	$(CC) -o  $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -g -o $@ $<

clean:
	rm -f $(objects)

clobber: clean
	rm -f http_generate_response
