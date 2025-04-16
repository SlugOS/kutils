/**
 * This covers basic string functions
 */

#include <stdint.h>
#include <stddef.h>

/* GCC and Clang reserve the right to generate calls to the following 4 functions even if they are not directly called. */

/**
 * @brief Copy one segment of memory to another
 * 
 * @param dest The destination or target of src
 * @param src This is the data we copy
 * @param n The size/amount of data copied so we know when to stpo
 * @return void* This returns the destination
 */
void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

/**
 * @brief This sets a place in memory to be a value
 * 
 * @param s The source
 * @param c The target
 * @param n The amount
 * @return void* The stuff we input
 */
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

/**
 * @brief Move one part of memory to another
 * 
 * @param dest The destination we move to
 * @param src The source that we move to the destination
 * @param n The size of what we copy
 * @return void* The pointer to the destination
 */
void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

/**
 * @brief Compare a place in memory
 * 
 * @param s1 The first place to compare
 * @param s2 The second place to compare
 * @param n The amount of both places we want to compare
 * @return int 0 if they are equal 1 if they are unequal
 */
int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

/* End of the required functions */

size_t strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

char *strcpy(char *dest, const char *src) {
    size_t i = 0;
    while ((dest[i] = src[i]) != '\0') {
        i++;
    }
    return dest;
}

char *strcat(char *dest, const char *src) {
    size_t dest_len = strlen(dest);
    size_t i = 0;

    // Append src to the end of dest
    while ((dest[dest_len + i] = src[i]) != '\0') {
        i++;
    }

    return dest;
}
