/**
 * A4
 *
 * COMP 2160 SECTION A02
 * INSTRUCTOR    John Braico
 * ASSIGNMENT    Assignment 4,
 * AUTHOR        Eddie Butkaliuk, 007910587
 * DATE          Dec 13th, 2021
 *
 * PURPOSE:  To thoroghly test the regions structure
 *          using automated tests
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "regions.h"

static int tests_done = 0;
static int tests_passed = 0;

void test_create_region(char *name, size_t size, Boolean expected)
{
    Boolean result = rinit(name, size);
    
    if(expected == result)
    {
        tests_passed++;
        printf("Passed create_region %s\n", name);
    }
    else
    {
        printf("FAILED create_region %s size:%lu, expected %d\n", name, size, expected);
    }
    tests_done++;
}

void check_chosen(char* expected)
{
    const char* chosen = rchosen();
    if((expected == NULL && chosen == NULL) || (!strcmp(chosen, expected)))
    {
        tests_passed++;
        printf("Passed check_chosen %s\n", expected);
    }
    else
    {
        printf("FAILED check_chosen expected:%s result:%s\n", expected, chosen);
    }
    tests_done++;
}

void test_destroy_region(char *name)
{
    rdestroy(name);
    if(!rchoose(name))
    {
        tests_passed++;
        printf("Passed destory_region %s\n", name);
    }
    else
    {
        printf("FAILED destory_region %s\n", name);
    }
     
    tests_done++;
    
    
}

void test_choose_region(char *name, Boolean expected)
{
    Boolean result = rchoose(name);
    if(result == expected)
    {
        tests_passed++;
        printf("Passed choose_region %s\n", name);
    }
    else
    {
        printf("FAILED choose_region %s expected %d\n", name, expected);
    }
    tests_done++;
}

void test_regions(void)
{
    printf("\nTESTING REGIONS\n");
    
    test_choose_region("Region6", FALSE);
    test_choose_region("Region1", FALSE);
    
    //test creating region (and choosing)
    check_chosen(NULL);
    test_create_region("Region1", 700, TRUE);
    test_create_region("Region2", 800, TRUE);
    check_chosen("Region2");
    test_create_region("Region3", 1, TRUE);
    test_create_region("Region4", 860, TRUE);
    test_create_region("Region5", 1254, TRUE);
    test_create_region("Region6", 8, TRUE);
    check_chosen("Region6");
    test_create_region("", 680, FALSE);
    test_create_region(NULL, 680, FALSE);
    test_create_region("Error", 0, FALSE);
    check_chosen("Region6");
    
    //test choose region
    test_choose_region("Region6", TRUE);
    test_choose_region("Region1", TRUE);
    test_choose_region("Region2", TRUE);
    test_choose_region("Region3", TRUE);
    test_choose_region("Region61", FALSE);
    test_choose_region(NULL, FALSE);
    test_choose_region("Region6", TRUE);
    test_choose_region("", FALSE);

    
    //test deleting region (and choosing)
    test_destroy_region("Region3");
    test_destroy_region("Region1");
    test_destroy_region("Region2");
    test_destroy_region("Region6");
    test_destroy_region("Region5");
    test_destroy_region("Region4");
    test_destroy_region("Region4");
    test_destroy_region("Error");
    test_destroy_region("");
    test_destroy_region(NULL);
    check_chosen(NULL);
    
    test_choose_region("Region6", FALSE);
    test_choose_region("Region1", FALSE);
    test_choose_region("Region3", FALSE);
    test_choose_region(NULL, FALSE);
    
}

void *test_allocate_block_wptr(size_t size, void *expected)
{
    void* result = ralloc(size);
    
    if(result == expected)
    {
        tests_passed++;
        printf("Passed allocate_block (with ptr) size %lu\n", size);
    }
    else
    {
        printf("FAILED allocate_block size %lu region %s\n", size, rchosen());
    }
    
    tests_done++;
    return result;
}

void *test_allocate_block_noptr(size_t size, Boolean expecting_to_work)
{
    void* result = ralloc(size);

    if((expecting_to_work && NULL != result) || (!expecting_to_work && NULL == result))
    {
        tests_passed++;
        printf("Passed allocate_block (no ptr) size %lu\n", size);
    }
    else
    {
        printf("FALILED allocate_block (no ptr) size %lu region %s\n",size, rchosen());
        printf("\texpecting %d result %p\n", expecting_to_work, result);
    }
    tests_done++;
    return result;
}

void test_free_block(void *ptr, Boolean expected)
{
    Boolean result = rfree(ptr);
    
    if(expected == result)
    {
        tests_passed++;
        printf("Passed block_free\n");
    }
    else
    {
        printf("FAILED block_free %p, region %s, expected %d\n", ptr, rchosen(), expected);
    }
    tests_done++;
}

void test_block_size(void *ptr, size_t expected)
{
    size_t result = rsize(ptr);
    
    if(result == expected)
    {
        tests_passed++;
        printf("Passed block_size\n");
    }
    else
    {
        printf("FAILED block_size %ptr expected %lu result %lu\n",ptr, expected, result);
    }
    tests_done++;
}

void test_blocks(void)
{
    printf("\nTESTING BLOCKS\n");
    
    //make a small region and test it
    test_create_region("RegionXS", 1, TRUE);
    test_allocate_block_wptr(9, NULL);
    void *ptr1, *ptr2, *ptr3, *ptr4, *ptr5, *ptr6;
    ptr1 = test_allocate_block_noptr(8, TRUE);
    ptr2 = test_allocate_block_noptr(1, FALSE);
    test_free_block(ptr1, TRUE);
    test_free_block(ptr2, FALSE);
    ptr3 = test_allocate_block_wptr(8, ptr1);
    
    //make another region and test it
    test_create_region("RegionM", 800, TRUE);
    ptr1 = test_allocate_block_noptr(400, TRUE);
    ptr2 = test_allocate_block_noptr(200, TRUE);
    ptr3 = test_allocate_block_noptr(200, TRUE);
    test_free_block(ptr2, TRUE);
    ptr2 = test_allocate_block_wptr(104, ptr2);
    ptr4 = ptr3;
    ptr3 = test_allocate_block_wptr(96, ptr2+104);
    test_allocate_block_noptr(0, FALSE);
    test_free_block(ptr1, TRUE);
    ptr1 = test_allocate_block_wptr(400, ptr1);
    test_free_block(ptr4, TRUE);
    test_allocate_block_wptr(208, NULL);
    ptr4 = test_allocate_block_wptr(112, ptr4);
    ptr5 = test_allocate_block_wptr(80, ptr4+112);
    ptr6 = test_allocate_block_wptr(8, ptr5+80);
    test_allocate_block_noptr(8, FALSE);
    test_allocate_block_noptr(1, FALSE);
    test_allocate_block_noptr(0, FALSE);
    
    
    //end
    test_destroy_region("RegionM");
    test_destroy_region("RegionXs");

    test_choose_region("RegionM", FALSE);
    
}

int main()
{
    test_regions();
    test_blocks();
    
    
    printf("\nTesting complete.\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests done: %d\n", tests_done);
    
  return 0;
}
