#include <stdint.h>
#include <stddef.h>

// Memory pool of 4KB
#define MEMORY_SIZE 4096
static uint8_t memory[MEMORY_SIZE];

// Block header structure
typedef struct block_header {
    size_t size;           // Size of the data area (not including header)
    uint8_t is_free;       // 1 if block is free, 0 if allocated
    struct block_header* next; // Pointer to the next block
} block_header_t;

// Initialize the memory pool with a single free block
static block_header_t* head = NULL;

// Function to initialize our memory allocator
void init_allocator() {
    if (head != NULL) return; // Already initialized
    
    // Create the initial block covering the entire memory pool
    head = (block_header_t*)memory;
    head->size = MEMORY_SIZE - sizeof(block_header_t);
    head->is_free = 1;
    head->next = NULL;
}

// Find a free block of sufficient size using first-fit algorithm
static block_header_t* find_free_block(block_header_t** last, size_t size) {
    block_header_t* current = head;
    
    while (current && !(current->is_free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    
    return current;
}

// Split a block if it's much larger than needed
static void split_block(block_header_t* block, size_t size) {
    // Only split if the difference would be large enough to hold a new block
    if (block->size < size + sizeof(block_header_t) + 8) {
        return; // Not worth splitting
    }
    
    // Create a new block after the allocated region
    block_header_t* new_block = (block_header_t*)((uint8_t*)block + sizeof(block_header_t) + size);
    new_block->size = block->size - size - sizeof(block_header_t);
    new_block->is_free = 1;
    new_block->next = block->next;
    
    // Update the current block
    block->size = size;
    block->next = new_block;
}

// Custom malloc implementation
void* malloc(size_t size) {
    if (size == 0) return NULL;
    
    // Initialize the allocator if needed
    if (head == NULL) {
        init_allocator();
    }
    
    // Ensure alignment
    size = (size + 7) & ~7; // Round up to multiple of 8
    
    block_header_t* last = NULL;
    block_header_t* block = find_free_block(&last, size);
    
    if (block == NULL) {
        return NULL; // No suitable block found
    }
    
    // Split the block if it's much larger than needed
    split_block(block, size);
    
    // Mark the block as allocated
    block->is_free = 0;
    
    // Return pointer to the data area
    return (uint8_t*)block + sizeof(block_header_t);
}

// Custom calloc implementation
void* calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    
    // Check for overflow
    if (size != 0 && total_size / size != num) {
        return NULL;
    }
    
    void* ptr = malloc(total_size);
    
    if (ptr != NULL) {
        // Zero out the allocated memory
        uint8_t* byte_ptr = (uint8_t*)ptr;
        for (size_t i = 0; i < total_size; i++) {
            byte_ptr[i] = 0;
        }
    }
    
    return ptr;
}

// Helper function to merge adjacent free blocks
static void merge_free_blocks() {
    block_header_t* current = head;
    
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            // Merge the next block into the current one
            current->size += sizeof(block_header_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// Custom free implementation
void free(void* ptr) {
    if (ptr == NULL) return;
    
    // Get the block header from the pointer
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    
    // Mark the block as free
    block->is_free = 1;
    
    // Merge adjacent free blocks to prevent fragmentation
    merge_free_blocks();
}

// Custom realloc implementation
void* realloc(void* ptr, size_t size) {
    // Handle special cases
    if (ptr == NULL) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    // Ensure alignment
    size = (size + 7) & ~7; // Round up to multiple of 8
    
    // Get the current block header
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    
    // If current block is large enough, just use it
    if (block->size >= size) {
        // We could split the block here if it's much larger
        split_block(block, size);
        return ptr;
    }
    
    // Check if we can merge with the next block if it's free
    if (block->next != NULL && block->next->is_free && 
        (block->size + sizeof(block_header_t) + block->next->size) >= size) {
        
        // Merge with the next block
        block->size += sizeof(block_header_t) + block->next->size;
        block->next = block->next->next;
        
        // Split if the merged block is much larger than needed
        split_block(block, size);
        
        return ptr;
    }
    
    // Allocate a new block
    void* new_ptr = malloc(size);
    if (new_ptr == NULL) {
        return NULL; // Allocation failed
    }
    
    // Copy data from old block to new block
    uint8_t* src = (uint8_t*)ptr;
    uint8_t* dst = (uint8_t*)new_ptr;
    size_t copy_size = block->size < size ? block->size : size;
    
    for (size_t i = 0; i < copy_size; i++) {
        dst[i] = src[i];
    }
    
    // Free the old block
    free(ptr);
    
    return new_ptr;
}