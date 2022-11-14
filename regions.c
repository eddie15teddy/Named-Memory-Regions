/**
 * A4
 *
 * COMP 2160 SECTION A02
 * INSTRUCTOR    John Braico
 * ASSIGNMENT    Assignment 4,
 * AUTHOR        Eddie Butkaliuk, 007910587
 * DATE          Dec 13th, 2021
 *
 * PURPOSE:  To implement a strucuture capable of
 *          creating named mememory regions. Each region can be selected
 *          deselected (by selecting a different region) and destroyed.
 *          each region can allocate memory blocks, and return pointers to them
 *          memory blocks can be deleted. All fucntions are managed here.
 */


#include "regions.h"
#include <stdio.h>
#include <string.h>

//this struct represents memory block as a node
typedef struct BLOCK
{
    void *ptr;
    rsize_t size;
    
    struct BLOCK *next;
}block;

//this struct represents a region as a node
typedef struct REGION
{
    char *name;
    void * start_ptr;
    void * end_ptr;
    
    rsize_t size;
    rsize_t free_space;
    struct REGION *next_region;
    
    block *first_block; //pointer to the first block of memory in this region
}region;

region *top = NULL;
region *bottom = NULL;
region *chosen = NULL;

rsize_t round_to_factor_8(rsize_t region_size)
{
    //adjust size to be a multiple of 8
    if(region_size%8 != 0)
    {
        region_size += 8 - (region_size%8);
    }
    
    return region_size;
}

//check if passed name is a possible region name
//return true if it is, false otherwise
Boolean validate_region_name(const char* name)
{
    if(name != NULL && strlen(name))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}//validate_region_name


//initializes a region with passed name and size
//size is rounded up to nearest number divisible by 8
Boolean rinit(const char *region_name, rsize_t region_size)
{
    //adjust size to be a multiple of 8
   region_size = round_to_factor_8(region_size);
    
    //make a region struct
    region *reg = NULL;
    reg = malloc(sizeof(region));
    
    if(!validate_region_name(region_name) || !region_size)
    {
        reg = NULL;
    }
    
    if(NULL != reg)
    {
        //allocate the memory for the region
        reg->start_ptr = NULL;
        reg->start_ptr = malloc(region_size);
        
        if(NULL != reg ->start_ptr)
        {
            //save the details about this region
            reg->name = malloc(sizeof(char) *strlen(region_name));
            strcpy(reg->name, region_name);
            reg->size = region_size;
            reg->free_space = region_size;
            reg->next_region = NULL;
            reg->first_block = NULL;
            reg->end_ptr = reg->start_ptr+region_size;
            
            //add this to the list of regions
            if(NULL == top)
            {
                top = reg;
                bottom = reg;
            }
            else
            {
                bottom->next_region = reg;
                bottom = reg;
            }
            
            //choose the newly created region
            chosen = reg;
            
            return TRUE;
        }
        else //free the region if could not allocate space for it
        {
            free(reg);
        }
    }
    
    //if this point is reached, one of the allocations failed
    return FALSE;
}//rinit

//chooses the region with passed name
//return false if region is not found
//true otherwise
Boolean rchoose(const char *region_name)
{
    Boolean found = FALSE;
    
    if(validate_region_name(region_name))
    {
        region *curr = top;
        
        while(NULL != curr && !found)
        {
            if(strcmp(region_name, curr->name) == 0)
            {
                found = TRUE;
                chosen = curr;
            }
            else
            {
                curr = curr->next_region;
            }
        }
    }
    
    return found;
}//rchoose

//return the name of the currently chosen region
//or null if no region is chosen
const char *rchosen(void)
{
    if(NULL == chosen)
    {
        return NULL;
    }
    else
    {
        return chosen -> name;
    }
}//rchosen

//frees the region with passed name
//and all memory associated with it
void rdestroy(const char *region_name)
{
    if(validate_region_name(region_name))
    {
        //find the region
        region *curr = top;
        region *prev = NULL;
        Boolean found = FALSE;
        while(NULL != curr && !found)
        {
            if(strcmp(region_name, curr->name) == 0)
            {
                found = TRUE;
            }
            else
            {
                prev = curr;
                curr = curr->next_region;
            }
        }
        
        //if region was found, found == true
        if(found)
        {
            //remove from list
            if(NULL == prev)
            {//the region to delete is the top one
                top = curr->next_region; //the next one is new top
            }
            else
            {//the region to delete is not the top
                prev->next_region = curr -> next_region;    //link around this region
                
                //if previous is now followed by NULL, it is the last one
                if(NULL == prev->next_region)
                {
                    bottom = prev;      //set bottom to point to prev
                }
            }
            
            //remove pointer from chosen
            if(NULL != chosen && strcmp(chosen->name, region_name) == 0)
            {
                chosen = NULL;
            }
            
            //free memory
            free(curr->name);
            free(curr->start_ptr);
            free(curr);
            
        }
    }
}//rdestroy

//allocate passed size of space from the chosen region
//return the pointer to that space
//manage that space
void *ralloc(rsize_t block_size)
{
    void *ptr = NULL;
    if(NULL != chosen && block_size>0)
    {
        //adjust block size
        block_size = round_to_factor_8(block_size);
        
        
        if(block_size <= chosen->free_space)
        {
            //malloc the block
            block *new_block = malloc(sizeof(block));
            new_block ->size = block_size;
            
            //find where to insert
            block* curr = chosen->start_ptr;
            block* next = chosen->first_block;
            
            
            if(NULL == next)
            {//adding the very first block to this region
                
                chosen->first_block = new_block;
                chosen->free_space = chosen->size - block_size;
                new_block->next = NULL;
                new_block->ptr = chosen->start_ptr;
                ptr = new_block->ptr;
                
            }
            else
            {//there are already existing blocks in this region
                if(curr == next->ptr)
                {//there is no space between first block and begining of the regions. Next needs to be moved
                    curr = next;
                    next = next->next;
                }
                
                //find where to insert
                Boolean found = FALSE;
                Boolean enough_space = FALSE;
                
                Boolean loop_exc = FALSE;
                
                while(NULL != next && !found)
                {
                    loop_exc = TRUE;
                    if((curr == chosen->start_ptr) && (next->ptr - (void*)curr) >= block_size)
                    {//curr is poniting to the start, not the first block
                        found = TRUE;
                        
                        if((void*)curr + block_size <= chosen->end_ptr)
                        {
                            enough_space = TRUE;
                        }
                    }
                    else if((curr != chosen->start_ptr)&&(next->ptr - (curr->ptr+curr->size)) >= block_size)
                    {
                        found = TRUE;
                        if(curr->ptr+curr->size+block_size <= chosen->end_ptr)
                        {
                            enough_space = TRUE;
                        }
                    }
                    else
                    {
                        //move along
                        curr = next;
                        next = next->next;
                    }
                }//while
                
                
                //see if there is enough space
                //either between end of current and end,
                //or between end of current and start of next
                
                if(curr->next == NULL && (curr->ptr + curr->size + block_size) <= chosen->end_ptr)
                {
                    enough_space = TRUE;
                }
                else if (curr->next != NULL && (curr->ptr+block_size) < next->ptr)
                {
                    enough_space = TRUE;
                }
                
                if(enough_space)
                {
                
                    //if the new block is should be the first in the list,
                    if(curr == chosen->start_ptr && curr != chosen->first_block->ptr)
                    {//set first to point to it
                        assert(chosen->start_ptr + block_size <= chosen->end_ptr);
                        chosen->first_block = new_block;
                        new_block->ptr = chosen->start_ptr;
                    }
                    else
                    {
                        if(curr == chosen -> start_ptr)
                        {//if here, curr is still pointing to the start of memory region, not the actual first bloc struct
                            curr = chosen -> first_block;
                        }
                        assert(curr->ptr + block_size <= chosen->end_ptr);
                        curr->next = new_block;
                        new_block->ptr = curr->ptr+curr->size;
                    }
                    
                    //insert this block into this region
                    new_block->next = next;
                    chosen->free_space = chosen->free_space - block_size;
                                
                    ptr = new_block->ptr;
                }
            }
        }//if enough space
    }
    return ptr;
}//ralloc

//returns the size of the block passed in chosen reggion
//0 on error
rsize_t rsize(void *block_ptr)
{
    rsize_t sz = 0;
    
    if(NULL != chosen && NULL != block_ptr)
    {
        block *curr = chosen->first_block;
        while(NULL != curr && curr->ptr != block_ptr)
        {
            curr = curr->next;
        }
        
        if(NULL != curr)
        {
            sz = curr -> size;
        }
    }
    
    return sz;
}//rsize

//removes the block pointed to by passed pointer
Boolean rfree(void *block_ptr)
{
    Boolean result = FALSE;
    
    //find the block with that pointer
    block *prev = NULL;
    block *curr = chosen->first_block;
    while(NULL != curr && !result)
    {
        if(curr->ptr == block_ptr)
        {
            result = TRUE;
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
        
    }
    
    //relink
    if(result)
    {
        chosen->free_space += curr->size;
        if(NULL == prev)
        {//removing first block
            chosen->first_block = curr->next;
        }
        else
        {//removing middle or last block
            prev->next = curr->next;
        }
        
        free(curr);
    }
    
    return result;
}//rdestroy

void rdump()
{
    region *curr_region = top;
    block *curr_block;
    while(NULL != curr_region)
    {
        curr_block = curr_region->first_block;
        printf("Region name: %s\n", curr_region->name);
        printf("Region size: %lu\n", curr_region->size);
        
        rsize_t free_space = curr_region->free_space;
        double size = (double)curr_region->size;
        double proportion = free_space/size;
        double percentage = proportion*100;
        
        
        printf("Free percentage: %f%%\n", percentage);
        while(NULL != curr_block)
        {
            printf("\t Block %p Size %lu\n", curr_block->ptr, curr_block->size);
            curr_block = curr_block->next;
        }
        
        curr_region = curr_region->next_region;
        printf("\n");
        
    }
}//rdump
