NVCC = nvcc

all: matmul

%.o : %.cu
	$(NVCC) -c $< -o $@

matmul : matmul.o
	$(NVCC) $^ -o $@

clean:
	rm -rf *.o *.a matmul
