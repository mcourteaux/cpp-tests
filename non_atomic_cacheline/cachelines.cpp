#include <stdio.h>
#include <memory>
#include <thread>
#include <atomic>

std::atomic_bool stop;

void writer(volatile int *array, int offset) {
    volatile int *ptr = reinterpret_cast<volatile int *>(
        reinterpret_cast<volatile char *>(array) + offset);
    while (!stop) {
        ptr[0] = 0;
        ptr[0] = -1;
    }
}

void reader(volatile int *array, int offset) {
    volatile int *ptr = reinterpret_cast<volatile int *>(
        reinterpret_cast<volatile char *>(array) + offset);
    int loops = 0;
    for (int i = 0; i < 10; loops++) {
        int copy = ptr[0];
        if (copy != 0 && copy != -1) {
            i++;
            printf("Found a shear read: %x\n", copy);
        }
    }
    printf("Loops needed: %d\n", loops);
}

int main() {
    const int CACHELINES = 3;
    int array[64 * CACHELINES / sizeof(int)];
    memset(array, 0, sizeof(array));

    int mod64 = ((long long unsigned int)(void *)array) % 64;
    printf("Array:\n");
    printf("  addr = %p\n", array);
    printf("  size = %zu\n", sizeof(array));
    printf("  addr %% 64 = %d\n", mod64);
    printf("  62 - (addr %% 64) = %d\n", 62 - mod64);
    printf("\n");

    int offset = 62 - mod64;
    printf("Enter a byte offset to start writing in the array. In order \n");
    printf("to write over two a cacheline boundary, enter %d\n", offset);
    printf("Byte offset: ");
    scanf("%d", &offset);

    int *ptr = array;
    stop = false;
    std::thread tw(writer, ptr, offset);
    std::thread tr(reader, ptr, offset);
    tr.join();
    stop = true;
    tw.join();

    printf("Result: (16 bytes per line)\n");
    // Print the memory byte by byte
    char *byte_ptr = reinterpret_cast<char *>(ptr);
    for (int i = 0; i < CACHELINES * 4; ++i) {
        for (int k = 0; k < 16; ++k) {
            char *a = &byte_ptr[i * 16 + k];
            // Color the cachelines alternatively
            if (((((long long unsigned int)(void *)a) / 64) & 1) == 0) {
                printf("\033[31m");
            } else {
                printf("\033[34m");
            }
            printf(" %02x", (*a) & 0xff);
        }
        printf("\n");
    }
    printf("\033[39;49m");
}
