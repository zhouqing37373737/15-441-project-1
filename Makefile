
default: lisod

echo_server:
	@gcc lisod.c -o lisod -Wall -Werror

clean:
	@rm lisod
