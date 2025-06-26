gcc -pthread -o rift_integrated_pipeline rift_integrated_pipeline.c
./rift_integrated_pipeline -v -i 'R"/let result = (x + y) * 42;/gmi[bt]"'
