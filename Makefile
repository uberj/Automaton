all: 
	gcc -g main.c -o run
pic: 
	gcc -g main.c -o run
	./run data
	python gen_pic.py data
