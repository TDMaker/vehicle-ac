main:
	gcc lsss.c operator.c -lgmp -lpbc
test_operator:
	cd rbs && gcc ./test_operator.c ../operator.c -o test_operator.out && ./test_operator.out && cd ..